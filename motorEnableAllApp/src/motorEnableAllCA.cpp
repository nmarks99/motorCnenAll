#include <cadef.h>
#include <string>

double get_mres(const std::string &pv);

double get_mres(const std::string &pv) {
    double mres;
    chid motor_chid;

    ca_context_create(ca_enable_preemptive_callback);
    std::string pv_name = pv + ".MRES"; 
    ca_create_channel(pv_name.c_str(), NULL, NULL, 10, &motor_chid);
    ca_pend_io(5.0);
    ca_get(DBR_DOUBLE, motor_chid, (void *)&mres);
    ca_pend_io(5.0);
    return mres;
}
