#pragma once

class D2GI;

class D2GIBase
{
protected:
	D2GI* m_pD2GI;
public:
	D2GIBase(D2GI* pD2GI) : m_pD2GI(pD2GI) {}

	D2GI* GetD2GI() { return m_pD2GI; }
};