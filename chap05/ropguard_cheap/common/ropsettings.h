// ropsettings.h
//

#define ADDR_SCRAMBLE_KEY    0x55555555

#define RG_MODULE_NAME_LEN   32
#define RG_FUNCTION_NAME_LEN 64

// stores information about a critical function
struct ROPGuardedFunction
{
	// name of the module in which the function is implemented
	// name of the critical function to guard
	char moduleName[RG_MODULE_NAME_LEN];
	char functionName[RG_FUNCTION_NAME_LEN];

	// original address of the function in the module
	// address of the corresponding guarded function
	unsigned long originalAddress;
	unsigned long patchedAddress;

	int stackIncrement;
};

// contains ROPGuard's configuration
class ROPSettings
{
private:
	ROPGuardedFunction *guardedFunctions;
	int numGuardedFunctions;

public:

	unsigned long preserveStack;

	ROPSettings()
	{
		preserveStack       = 4;
		guardedFunctions    = 0;
		numGuardedFunctions = 0;
	}

	ROPGuardedFunction *GetGuardedFunctions()
	{
		return guardedFunctions;
	}
	
	int GetNumGuardedFunctions()
	{
		return numGuardedFunctions;
	}

	// adds a critical function
	//   moduleName:     name of the module that contains the critical function
	//   functionName:   name of the critical function
	//   stackIncrement: how many DWORDS does function take from the stack (-1 if don't know)
	//   protect:        if true, ROPCheck will be called whenever the function is called
	//   clearCache:     if true, when this function gets called, executable module cache will be cleared
	void AddFunction(
		const char *moduleName, const char *functionName, int stackIncrement);
};

// reads and parses the settings file
int ReadROPSettings(char *filename);

// reads the settings from 'ropsettings.txt' file in the same folder as the loaded 'ropguarddll.dll'
int ReadROPSettings();

// returns the number of critical functions
int GetNumGuardedFunctions();

// returns the pointer to the array of ROPGuardedFunction objects that contain information about critical functions
ROPGuardedFunction *GetGuardedFunctions();

// returns the ropSettings object
ROPSettings *GetROPSettings();
