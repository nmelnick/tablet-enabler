
#include "ThinkPadTabletEnabler.h"
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/IOPlatformExpert.h>
#include <IOKit/IODeviceMemory.h>
#include <IOKit/IODeviceTreeSupport.h>
#include <IOKit/IORangeAllocator.h>
#include <IOKit/acpi/IOACPITypes.h>

#define super IOService
OSDefineMetaClassAndStructors(com_dytara_driver_thinkpadtabletenabler, IOService);

IOService *TabletEnabler::
probe(IOService *provider, SInt32 *score)
{
    char *Name = "TabletEnabler";
    Provider = OSDynamicCast(IOACPIPlatformDevice, provider);
    if (!Provider) {
        IOLog("%s: Attached to non-IOACPIPlatformDevice provider!  Failing probe()\n", Name);
        return NULL;
    }

	// _SRS fire
	OSObject *params;
	OSObject *result;
	OSObject *paramArray[1];
	IOItemCount paramCount = 1;
	IOOptionBits options = 1;
	IOReturn status;
	
	status = Provider->evaluateObject(
		"_CRS",
		&params
	);
	
	OSData *dat = OSDynamicCast(OSData, params);
	IOLog("%s: _CRS status: 0x%x params-length: %d\n", Name, status, dat->getLength());
	
	paramArray[0] = params;
	status = Provider->evaluateObject(
		"_SRS",
		&result,
		paramArray,
		paramCount,
		options
	);
	
	IOLog("%s: _SRS status: %#x, result: %#x\n", Name, status, result);
	
	int Indexes = 0;

	IOLog("%s: Initial State: DeviceType %#x, DeviceStatus %#x\n", Name, Provider->getDeviceType(), Provider->getDeviceStatus());
	
	Indexes = int(provider->getDeviceMemoryCount());
	IOLog("%s: %d memory indexes available\n", Name, Indexes);
	
	if (Indexes == 0) {
		IOLog("%s: Manually allocating resources.\n", Name);
		
		IORangeAllocator *platformRanges;
		OSArray *rangeArray;
		IODeviceMemory::InitElement rangeList[1];
		IODeviceMemory *ioMemory;
		
		rangeList[0].start = 0x00000200;
		rangeList[0].length = 8;
		platformRanges = getPlatform()->getPhysicalRangeAllocator();
		platformRanges->allocateRange( rangeList[0].start, rangeList[0].length );
		
		rangeArray = IODeviceMemory::arrayFromList( rangeList, 1 );
		provider->setDeviceMemory( rangeArray );
		rangeArray->release();
		
		ioMemory = (IODeviceMemory *) rangeArray->getObject( 0 );

		IOLog("%s: Interrupts: Constants.\n", Name);
		const OSSymbol *controller = OSSymbol::withCStringNoCopy("io-apic-0");
		const OSSymbol *contKey = OSSymbol::withCStringNoCopy("IOInterruptControllers");
		const OSSymbol *specKey = OSSymbol::withCStringNoCopy("IOInterruptSpecifiers");
		
		IOLog("%s: Interrupts: Allocate Controller.\n", Name);
		OSArray *controllers = OSArray::withCapacity(1);
		controllers->setObject(controller);
		IOLog("%s: Interrupts: Set Controller.\n", Name);
		provider->setProperty( contKey, controllers );
		
		IOLog("%s: Interrupts: Allocate Specifider.\n", Name);
		OSArray *mapped = OSArray::withCapacity(1);
		OSData *map = OSData::withCapacity(8);
		map->appendByte( 05, 1 );
		int i = 0;
		for (i = 0; i < 7; i++) {
			map->appendByte( 00, 1 );
		}
		mapped->setObject( map );
		IOLog("%s: Interrupts: Set Specifier.\n", Name);
		provider->setProperty( specKey, mapped );
		
		IOLog("%s: Interrupts: DeviceTree Map provider.\n", Name);
		IODTMapInterrupts( provider );
		
		status = resolveInterrupt(provider, 0);
		IOLog("%s: Interrupts: resolve: %#x\n", Name, status);
	}

    return this;
}

bool TabletEnabler::
start(IOService *provider)
{
	IOLog("TabletEnabler: start\n");
	IOLog("TabletEnabler: Initial State: DeviceType 0x%x, DeviceStatus 0x%x\n", Provider->getDeviceType(), Provider->getDeviceStatus());
	
	Map = provider->mapDeviceMemoryWithIndex(0);
    if (!Map) {
		IOLog("TabletEnabler: Unable to map device memory\n");
    }
	
	return false;
}

void TabletEnabler::
stop(IOService *provider)
{
    IOLog("TabletEnabler: stop(%p)\n", provider);
    super::stop(provider);
}

void TabletEnabler::
free()
{
    IOLog("TabletEnabler: free()\n");
    super::free();
}


