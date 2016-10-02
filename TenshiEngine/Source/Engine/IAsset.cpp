#include "IAsset.h"


IAsset::IAsset() {
	m_Ptr = NULL;
	memset(&m_Hash, 0, sizeof(m_Hash));
	m_Name = "";
}
IAsset::~IAsset() {}
void IAsset::Load(MD5::MD5HashCoord hash) {
	(void)hash;
}
void IAsset::Free() {
	m_Ptr = NULL;
	memset(&m_Hash, 0, sizeof(m_Hash));
	m_Name = "";
}
bool IAsset::IsLoad()const {
	return m_Ptr != NULL;
}

