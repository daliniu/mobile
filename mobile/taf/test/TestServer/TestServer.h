#include "servant/Application.h"
#include "Test.h"

using namespace taf;
using namespace Test;

/**
 * 
 */
class TestServer : public Application
{
protected:

    /**
     * Îö¹»
     */
    virtual void initialize();

    /**
     *
     */
    virtual void destroyApp();

protected:
    /**
     * 
     * @param command
     * @param params
     * @param result
     * 
     * @return bool
     */
    bool procAdmin(const string& command, const string& params, string& result);
};


