#ifndef __TEST_H_
#define __TEST_H_

#include "Jce_sym.h"
#include "sys/types.h"
#include "netinet/in.h"


namespace Test
{
    struct TestInfo : public JceStructBase
    {
    public:
        static taf::String className()
        {
            return "Test.TestInfo";
        }
        static taf::String MD5()
        {
            return "1f3daf5bfd54cffdcb406653c14acf2f";
        }
        TestInfo()
        : b(true),si(0),by(0),ii(0),li(0),f(0),d(0),s("")
        {
        }
        void writeTo(taf::JceOutputStream& _os) const
        {
            _os.write(b, 1);
            _os.write(si, 2);
            _os.write(by, 3);
            _os.write(ii, 4);
            _os.write(li, 5);
            _os.write(f, 6);
            _os.write(d, 7);
            _os.write(s, 8);
            _os.write(vi, 9);
            _os.write(mi, 10);
        }
        void readFrom(taf::JceInputStream& _is)
        {
            _is.read(b, 1, true);
            _is.read(si, 2, true);
            _is.read(by, 3, true);
            _is.read(ii, 4, true);
            _is.read(li, 5, true);
            _is.read(f, 6, true);
            _is.read(d, 7, true);
            _is.read(s, 8, true);
            _is.read(vi, 9, true);
            _is.read(mi, 10, true);
        }
        void display(CConsoleBase * _con, int _level=0) const
        {
            taf::JceDisplayer _ds(_con, _level);
            _ds.display(b,"b");
            _ds.display(si,"si");
            _ds.display(by,"by");
            _ds.display(ii,"ii");
            _ds.display(li,"li");
            _ds.display(f,"f");
            _ds.display(d,"d");
            _ds.display(s,"s");
            _ds.display(vi,"vi");
            _ds.display(mi,"mi");
        }
        void displaySimple(CConsoleBase * _con, int _level=0) const
        {
            taf::JceDisplayer _ds(_con, _level);
            _ds.displaySimple(b, true);
            _ds.displaySimple(si, true);
            _ds.displaySimple(by, true);
            _ds.displaySimple(ii, true);
            _ds.displaySimple(li, true);
            _ds.displaySimple(f, true);
            _ds.displaySimple(d, true);
            _ds.displaySimple(s, true);
            _ds.displaySimple(vi, true);
            _ds.displaySimple(mi, false);
        }
    public:
        taf::Bool b;
        taf::Short si;
        taf::Char by;
        taf::Int32 ii;
        taf::Int64 li;
        taf::Float f;
        taf::Double d;
        taf::String s;
        taf::JArray<taf::Int32> vi;
        taf::JMapWrapper<taf::Int32, taf::String> mi;
    };
    inline bool operator==(const TestInfo&l, const TestInfo&r)
    {
        return l.b == r.b && l.si == r.si && l.by == r.by && l.ii == r.ii && l.li == r.li && l.f == r.f && l.d == r.d && l.s == r.s && l.vi == r.vi && l.mi == r.mi;
    }
    inline bool operator!=(const TestInfo&l, const TestInfo&r)
    {
        return !(l == r);
    }


}



#endif
