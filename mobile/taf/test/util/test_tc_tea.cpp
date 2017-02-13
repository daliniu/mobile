#include "util/tc_tea.h"
#include "util/tc_common.h"

using namespace taf;

#define KEY  "dRa93"

int main(int argc, char *argv[])
{
    try
    {
        string n = "abde";
        {
            string s = n;
            cout << "encrypt:" << s << endl;
            vector<char> v = taf::TC_Tea::encrypt(KEY, s.c_str(), s.length());
            cout << "encrypt:" << TC_Common::bin2str(&v[0], v.size()) << endl;
            vector<char> s1 = taf::TC_Tea::decrypt(KEY, &v[0], v.size());
            cout << "decrypt:" << &s1[0] << endl;
        }

        {
            string s = n;
            cout << "encrypt:" << s << endl;
            vector<char> v = taf::TC_Tea::encrypt2(KEY, s.c_str(), s.length());
            cout << "encrypt:" << TC_Common::bin2str(&v[0], v.size()) << endl;
            vector<char> s1 = taf::TC_Tea::decrypt2(KEY, &v[0], v.size());
            cout << "decrypt:" << &s1[0] << endl;
        }
	}
	catch(exception &ex)
	{
        cout << ex.what() << endl;
	}

	return 0;
}


