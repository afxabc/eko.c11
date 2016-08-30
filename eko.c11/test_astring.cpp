#include "base/AString.h"
#include "base/log.h"
#include <map>

using namespace eko;


void test_string()
{
    LOGI("\n======= test_astring =======");

	AString str("123");

	str += "aaaaaa";

	std::cout << str << std::endl;

	AString s;
	s.format("\t%.4X - %.3f", 1000, 0.02);

	str += (s+" ====\n                 ");

	s = str;
	std::cout << s << std::endl;

	s = "\t" + s;
	std::cout << s << std::endl;
	s.trimLeft();
	s.trimRight();
	std::cout << s << std::endl;

	s.insert(7, "XXX");
	std::cout << s << std::endl;

	s.insert(s.length(), "YYY");
	std::cout << s << std::endl;

	s.insert(s.size(), "ZZZ");
	std::cout << s << std::endl;

	s.insert(0, "AAA");
	std::cout << s << std::endl;

	s.erase(0, 8);
	std::cout << s << std::endl;

	s.erase(0, s.length());
	std::cout << s << std::endl;

	s = "a foG iS over CraZy.";
	str = "A fog is over crazy.";
	assert(s.sameNoCase(str));

	s = "A@fog is over crazy.";
	assert(!s.sameNoCase(str));

	s = "ttt";
	std::cout << s << std::endl;
	std::cout << str << std::endl;

	s.swap(str);
	std::cout << s << std::endl;
	std::cout << str << std::endl;

	typedef std::map<AString, int> UserMap;
	UserMap users_;
	users_["kate"] = 123456;
	users_["jack"] = 654321;
	std::cout << users_["kate"] << std::endl;
	std::cout << users_["tom"] << std::endl;

}
