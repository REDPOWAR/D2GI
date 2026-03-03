
#include "../common/logger.h"

#include "d2gi_backbuf_surf.h"
#include "d2gi.h"
#include "d2gi_enums.h"


D2GIBackBufferSurface::D2GIBackBufferSurface(D2GI* pD2GI, DWORD dwWidth, 
	DWORD dwHeight, D2GIPIXELFORMAT eFormat) 
	: D2GISurface(pD2GI, dwWidth, dwHeight, eFormat)
{
	LoadResource(/*bResettingDevice=*/ false);
}


D2GIBackBufferSurface::~D2GIBackBufferSurface()
{
}


void D2GIBackBufferSurface::ReleaseResource(bool bResettingDevice)
{
	D2GISurface::ReleaseResource(bResettingDevice);

	writing_texture_.Reset();
	writing_surface_.Reset();
	reading_surface_.Reset();
	if (!bResettingDevice)
		intermediate_surface_.Reset();
}


void D2GIBackBufferSurface::LoadResource(bool bResettingDevice)
{
	auto device = GetD3D9Device();

	if (FAILED(device->CreateTexture(m_dwWidth, m_dwHeight, 1, D3DUSAGE_DYNAMIC,
	                                 g_asD2GIPF_To_D3D9PF[m_eD2GIPixelFormat], D3D9::D3DPOOL_DEFAULT,
	                                 &writing_texture_, nullptr))) {
		Logger::Error(T("Failed to create backbuffer writing texture"));
	}

	if (FAILED(writing_texture_->GetSurfaceLevel(0, &writing_surface_)))
		Logger::Error(T("Failed to get backbuffer writing surface"));

	if (FAILED(device->CreateRenderTarget(m_dwWidth, m_dwHeight, g_asD2GIPF_To_D3D9PF[m_eD2GIPixelFormat],
	                                      D3D9::D3DMULTISAMPLE_NONE, 0, FALSE, &reading_surface_, nullptr))) {
		Logger::Error(T("Failed to create backbuffer reading render target"));
	}

	if (intermediate_surface_ == nullptr) {
		if (FAILED(device->CreateOffscreenPlainSurface(m_dwWidth, m_dwHeight,
		                                               g_asD2GIPF_To_D3D9PF[m_eD2GIPixelFormat],
		                                               D3D9::D3DPOOL_SYSTEMMEM, &intermediate_surface_, nullptr))) {
			Logger::Error(T("Failed to create backbuffer intermediate surface"));
		}
	}
}


HRESULT D2GIBackBufferSurface::Lock(LPRECT rect, D3D7::LPDDSURFACEDESC2 description, DWORD flags, HANDLE)
{
	if (rect != nullptr)
		return DDERR_GENERIC;

	// Backbuffer locking can be used by game for two scenarios: to read rendered scene (implementation
	// for wet windscreen refraction texture) or to write 2D frame directly (FMV playback).
	// Only one scenario may happen during one frame, so we can use common intermediate system-memory
	// surface both for writing and reading. We can detect scenario by flags.
	// Writing may flow in different ways. The simplest is when fullscreen FMV is playing, so
	// nothing else needed to be written into buffer, and we can stream data directly into GPU-memory
	// texture (bypassing intermediate system-memory one) - it will be very fast implementation.
	// More complex case is when in-game FMV is playing: as it's not fullscreen, the game fills background
	// by color before. This is when multi-part writing is used: first step is to fill whole intermediate
	// system-memory surface by color (see Blt() method of this class), the second is to lock this surface
	// and allow game to write FMV frame into it (we are here), and finally we copy intermediate surface
	// to GPU's one (see OnFlip() method of this class). In other words, in this case we are doing all
	// fill operations in system memory, then moving final result to GPU, so it will be fast enough too.
	bool is_in_reading = !(flags & DDLOCK_WRITEONLY);

	m_pD2GI->OnBackBufferLock(is_in_reading);

	D3D9::D3DLOCKED_RECT locked_rect;
	if (is_in_reading || is_in_multipart_writing_) {
		if (is_in_reading)
			GetD3D9Device()->GetRenderTargetData(reading_surface_.Get(), intermediate_surface_.Get());

		if (FAILED(intermediate_surface_->LockRect(&locked_rect, nullptr,
			                                       D3DLOCK_NOSYSLOCK | (is_in_reading ? D3DLOCK_READONLY : 0)))) {
			Logger::Error(T("Failed to lock backbuffer intermediate surface (on data streaming)"));
		}

		locked_surface_ = intermediate_surface_.Get();
	} else {
		if (FAILED(writing_surface_->LockRect(&locked_rect, nullptr, D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD)))
			Logger::Error(T("Failed to lock backbuffer writing surface"));

		locked_surface_ = writing_surface_.Get();
	}

	ZeroMemory(description, sizeof(*description));
	description->dwSize = sizeof(*description);
	description->dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH | DDSD_PIXELFORMAT | DDSD_LPSURFACE;
	description->ddsCaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_3DDEVICE
	                              | DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY;
	description->dwWidth = m_dwWidth;
	description->dwHeight = m_dwHeight;
	description->ddpfPixelFormat = m_sDD7PixelFormat;
	description->lPitch = locked_rect.Pitch;
	description->lpSurface = locked_rect.pBits;

	return DD_OK;
}


HRESULT D2GIBackBufferSurface::Unlock(LPRECT)
{
	locked_surface_->UnlockRect();
	locked_surface_ = nullptr;

	return DD_OK;
}


HRESULT D2GIBackBufferSurface::AddAttachedSurface(D3D7::LPDIRECTDRAWSURFACE7 pSurf)
{
	if (((D2GISurface*)pSurf)->GetType() == ST_ZBUFFER)
		return DD_OK;

	Logger::Warning(TEXT("Attaching unknown surface to backbuffer"));
	return DDERR_GENERIC;
}


HRESULT D2GIBackBufferSurface::Blt(LPRECT destination_rect,
                                   D3D7::LPDIRECTDRAWSURFACE7 source_surface, LPRECT source_rect,
                                   DWORD flags, D3D7::LPDDBLTFX effects)
{
	RECT using_destination_rect = destination_rect == nullptr
		? RECT{0, 0, (LONG)m_dwWidth, (LONG)m_dwHeight}
		: *destination_rect;

	if (flags & DDBLT_COLORFILL) {
		if (m_dwBPP != 16)
			return DDERR_GENERIC;

		// The game uses this color filling only once - when non-fullscreen FMV is playing (filling
		// is used to make background). Moreover, it happens right before the game starts filling
		// FMV rectangle in the middle of the backbuffer by locking and direct writing into
		// backbuffer's surface (the game locks whole surface), so at this moment we must keep
		// result of color filling inside the buffer, otherwise we can see random garbage that
		// surrounds FMV rectangle.
		// The fastest way to implement it is to fill system memory buffer by color using CPU and
		// then return same buffer to the game to write FMV data instead of drawing colored rectangle
		// by GPU and reading render target.
		// So, we use intermediate system-memory surface to do it (see full flow description in Lock()
		// method of this class).
		D3D9::D3DLOCKED_RECT locked_rect;
		if (FAILED(intermediate_surface_->LockRect(&locked_rect, &using_destination_rect,
		                                           D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD))) {
			Logger::Error(T("Failed to lock backbuffer intermediate surface (on color filling)"));
		}

		int rect_width = using_destination_rect.right - using_destination_rect.left;
		int rect_height = using_destination_rect.bottom - using_destination_rect.top;
		uint16_t color = (uint16_t)effects->dwFillColor;

		uint8_t* rows_end = (uint8_t*)locked_rect.pBits + locked_rect.Pitch * rect_height;
		for (uint8_t* row = (uint8_t*)locked_rect.pBits; row < rows_end; row += locked_rect.Pitch) {
			uint16_t* pixels_end = (uint16_t*)row + rect_width;
			for (uint16_t* current_pixel = (uint16_t*)row; current_pixel < pixels_end; current_pixel++)
				*current_pixel = color;
		}

		intermediate_surface_->UnlockRect();

		is_in_multipart_writing_ = true;

		return DD_OK;
	}

	if (source_surface == nullptr || ((D2GISurface*)source_surface)->GetType() != ST_SYSMEM)
		return DDERR_GENERIC;

	m_pD2GI->OnSysMemSurfaceBltOnBackBuffer((D2GISystemMemorySurface*)source_surface, source_rect,
	                                        this, &using_destination_rect);

	return DD_OK;
}


void D2GIBackBufferSurface::OnFlip()
{
	if (is_in_multipart_writing_) {
		GetD3D9Device()->UpdateSurface(intermediate_surface_.Get(), nullptr, writing_surface_.Get(), nullptr);
		is_in_multipart_writing_ = false;
	}
}
