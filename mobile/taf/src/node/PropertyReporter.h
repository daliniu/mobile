
//klarsepan for report property 2009-09-21

//eg1: REPORT_COUNT("socket_error", 1);
//eg2: REPORT_SUM("GB_consume", 100);


#ifndef  __PropertyReporter_H_
#define  __PropertyReporter_H_


#include "servant/Application.h"

//define reporter template
#define REPORT_INSTALL \
template<typename REPORTFUNC> \
PropertyReportPtr creatReport(const string & master, string msg, REPORTFUNC reportFunc) \
{ \
    PropertyReportPtr srp = Application::getCommunicator()->getStatReport()->getPropertyReport(msg); \
    if ( !srp ) \
    { \
        srp = Application::getCommunicator()->getStatReport()->createPropertyReport(msg, reportFunc); \
        srp->setMasterName(master);\
    } \
    return srp; \
};


//count report
#define REPORT_COUNT(master, str,value) \
PropertyReportPtr countReporter = creatReport(master, str, PropertyReport::count()); \
countReporter->report(value);


//sum report
#define REPORT_SUM(master, str,value) \
PropertyReportPtr sumReporter   = creatReport(master, str, PropertyReport::sum()); \
sumReporter->report(value);

//max report
#define REPORT_MAX(master, str,value) \
PropertyReportPtr maxReporter   = creatReport(master, str, PropertyReport::max()); \
maxReporter->report(value);

//set a reporter
REPORT_INSTALL;


#endif // __PropertyReporter_H_

