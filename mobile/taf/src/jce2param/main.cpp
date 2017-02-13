#include "util/tc_option.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include "parse/element.h"
#include "parse/parse.h"


void usage()
{
    cout << "Usage : jce2param jcefile" << endl;
    cout << "  jce2param support type: bool byte short int long float double vector map" << endl;
    exit(0);
}

void check(vector<string> &vJce)
{
    bool b = true;

    for(size_t i  = 0; i < vJce.size(); i++)
    {
        string ext  = taf::TC_File::extractFileExt(vJce[i]);
        if(ext == "jce")
        {
            if(!b)
            {
                usage();
            }
            if(!taf::TC_File::isFileExist(vJce[i]))
            {
                cerr << "file '" << vJce[i] << "' not exists" << endl;
                exit(0);
            }
        }
        else
        {
            cerr << "only support jce file." << endl;
            exit(0);
        }
    }
}

string describeType(TypePtr pPtr) 
{
    {
        StructPtr ptr=StructPtr::dynamicCast(pPtr);
        if(ptr)
        {
            string structStr="struct<";
            vector<TypeIdPtr>& member = ptr->getAllMemberPtr();
            for(size_t j = 0; j < member.size(); j++)
            {
                structStr+=describeType(member[j]->getTypePtr());
                structStr+=(j==member.size()-1?">":",");
            }
            return structStr;
        }
    }
    {
        MapPtr ptr=MapPtr::dynamicCast(pPtr);
        if (ptr)
        {
            return string("map<")+describeType(ptr->getLeftTypePtr())+","+describeType(ptr->getRightTypePtr())+">";
        }
    }
    {
        VectorPtr ptr=VectorPtr::dynamicCast(pPtr);
        if (ptr)
        {
            return string("vector<")+describeType(ptr->getTypePtr())+">";
        }
    }
    {
        EnumPtr ptr=EnumPtr::dynamicCast(pPtr);
        if(ptr)
        {
            return "int";
        }
    }
    {
        BuiltinPtr ptr=BuiltinPtr::dynamicCast(pPtr);
        if(ptr)
        {
            return ptr->builtinTable[ptr->kind()];
        }
    }
    return "ERROR";
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        usage();
    }

    taf::TC_Option option;
    option.decode(argc, argv);
    vector<string> vJce = option.getSingle();

    check(vJce);

    if(option.hasParam("help"))
    {
        usage();
    }

    //是否可以以taf开头
    g_parse->setTaf(option.hasParam("with-taf"));
    g_parse->setHeader(option.getValue("header"));


    ostringstream s;

    try
    {
        for(size_t i = 0; i < vJce.size(); i++)
        {

            g_parse->parse(vJce[i]);
            vector<ContextPtr> contexts= g_parse->getContexts();
            vector<NamespacePtr> namespaces=contexts[i]->getNamespaces();
            for(size_t j=0;j<namespaces.size();++j)
            {
                s<<"namesapce "<<namespaces[i]->getId()<<endl;
                s<<"{"<<endl;
                vector<InterfacePtr> interfaces=namespaces[i]->getAllInterfacePtr();
                for(size_t k=0;k<interfaces.size();++k)
                {
                    s<<"    interface "<<interfaces[i]->getId()<<endl;
                    s<<"    {"<<endl;
                    vector<OperationPtr> operations=interfaces[k]->getAllOperationPtr();
                    for(size_t l=0;l<operations.size();++l)
                    {
                        OperationPtr pPtr=operations[l];
                        vector<ParamDeclPtr> vParamDecl=pPtr->getAllParamDeclPtr();
                        string outparams;
                        string inparams;
                        for(size_t m =0; m < vParamDecl.size(); m++)
                        {
                            if(vParamDecl[m]->isOut())
                            {
                                if(outparams.empty())
                                {
                                    outparams+=describeType(vParamDecl[m]->getTypeIdPtr()->getTypePtr());
                                }
                                else
                                {
                                    outparams+="|"+describeType(vParamDecl[m]->getTypeIdPtr()->getTypePtr());
                                }
                            }
                            else
                            {
                                if(inparams.empty())
                                {
                                    inparams+=describeType(vParamDecl[m]->getTypeIdPtr()->getTypePtr());
                                }
                                else
                                {
                                    inparams+="|"+describeType(vParamDecl[m]->getTypeIdPtr()->getTypePtr());
                                }
                            }
                        }
                        s << "        /**"<<endl;
                        if(!inparams.empty())
                        {
                            s << "         * @in  param: "<<inparams<<endl;
                        }
                        if(!outparams.empty())
                        {
                            s << "         * @out param: "<<outparams<<endl;;
                        }
                        s << "         *"<<endl;
                        s << "         * @return: "<<describeType(pPtr->getReturnPtr()->getTypePtr())<<endl;
                        s << "         **/"<<endl;
                        s << "        " << describeType(pPtr->getReturnPtr()->getTypePtr()) << " " << pPtr->getId() << "(";
                        for(size_t m =0; m < vParamDecl.size(); m++)
                        {
                            if(vParamDecl[m]->isOut())
                            {
                                s<<"out ";
                            }
                            s << describeType(vParamDecl[m]->getTypeIdPtr()->getTypePtr());
                            s << (m==vParamDecl.size()-1?")":",");
                        }
                        s << endl <<endl;
                    }
                    s<<"    }"<<endl;
                }
                s<<"}"<<endl;
            }
        }
        cout<<s.str()<<endl;
    }catch(exception& e)
    {
        cerr<<e.what()<<endl;
    }

    return 0;
}

