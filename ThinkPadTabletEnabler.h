

#include <IOKit/IOService.h>   // provider class
#include <IOKit/acpi/IOACPIPlatformDevice.h>   // provider class

#define TabletEnabler com_dytara_driver_thinkpadtabletenabler
class TabletEnabler : public IOService
{
    OSDeclareDefaultStructors(com_dytara_driver_thinkpadtabletenabler);

public:
    virtual IOService *probe(IOService *provider, SInt32 *score);
    virtual bool start(IOService *provider);
    virtual void stop(IOService *provider);
    virtual void free();

protected:
    IOACPIPlatformDevice  *Provider;
    IOMemoryMap           *Map;
    IOPhysicalAddress     RegBase;
    const char            *Location;
};