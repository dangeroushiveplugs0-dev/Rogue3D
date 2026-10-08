#include "rogue/assets/rb_container.hpp"
namespace rogue::assets {
namespace {
void w(std::vector<std::uint8_t>& o,std::uint32_t v){o.push_back(v);o.push_back(v>>8);o.push_back(v>>16);o.push_back(v>>24);}
bool r(std::string_view s,std::size_t& p,std::uint32_t& v){if(p+4>s.size())return false;v=(unsigned char)s[p]|((std::uint32_t)(unsigned char)s[p+1]<<8)|((std::uint32_t)(unsigned char)s[p+2]<<16)|((std::uint32_t)(unsigned char)s[p+3]<<24);p+=4;return true;}
}
std::uint32_t RbContainer::fourcc(char a,char b,char c,char d) noexcept{return (unsigned char)a|((std::uint32_t)(unsigned char)b<<8)|((std::uint32_t)(unsigned char)c<<16)|((std::uint32_t)(unsigned char)d<<24);}
bool RbContainer::add_chunk(std::uint32_t type,std::vector<std::uint8_t> data){if(!type||data.size()>0xffffffffu)return false;chunks_.push_back({type,std::move(data)});return true;}
bool RbContainer::serialize(std::vector<std::uint8_t>& o) const{if(chunks_.size()>0xffffffffu)return false;o.clear();w(o,kMagic);w(o,kVersion);w(o,(std::uint32_t)chunks_.size());for(const auto& c:chunks_){w(o,c.type);w(o,(std::uint32_t)c.data.size());o.insert(o.end(),c.data.begin(),c.data.end());}return true;}
bool RbContainer::deserialize(std::string_view s){clear();std::size_t p=0;std::uint32_t m=0,v=0,n=0;if(!r(s,p,m)||!r(s,p,v)||!r(s,p,n)||m!=kMagic||v!=kVersion||n>100000)return false;for(std::uint32_t i=0;i<n;++i){std::uint32_t t=0,z=0;if(!r(s,p,t)||!r(s,p,z)||p+(std::size_t)z>s.size())return false;chunks_.push_back({t,std::vector<std::uint8_t>(s.begin()+p,s.begin()+p+z)});p+=z;}return p==s.size();}
}