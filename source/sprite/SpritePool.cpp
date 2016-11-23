//#include "SpritePool.h"
//#include "SpriteFactory.h"
//
//namespace gum
//{
//
//SINGLETON_DEFINITION(SpritePool);
//
//s2::Sprite* SpritePool::Create(int node_id)
//{
//	std::map<uint32_t, s2::Sprite*>::iterator itr 
//		= m_map_node_id.find(node_id);
//	if (itr != m_map_node_id.end()) {
//		return itr->second;
//	}
//
//	s2::Sprite* spr = SpriteFactory::Instance()->CreateFromSym(node_id);
//	m_map_node_id.insert(std::make_pair(node_id, spr));
//
//	return spr;
//}
//
//}