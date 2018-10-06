/*

INVOKE.exe COMn             (Transmit MSP430 BSL invoke pattern on DTR and RTS)

This program was written in C for the LCC-Win32 compiler.

*/

#include <stdio.h>
#include <conio.h>
#include <mmsystem.h>           /* Must also add winmm.lib to the Link. */

int comport = 0;
int filearg = 0;
int filelen = 0;
unsigned int pulsetime = 10;    /* Period in ms of first positive pulse on RTS */

/*======== Display proper usage of program if /?, -h or error. ===========*/

void Usage(char *programName)
{
    printf("\n%s usage:\n \n",programName);
    printf("Transmit BSL invoke pattern on DTR(/Reset) and RTS(Test):      %s COMn \n",programName);
}

/*======== Process command line arguments. ==========*/

void HandleOptions(int argc,char *argv[])
{
    int i;

    for (i=1; i< argc;i++)
    {
        if (argv[i][0] == '/' || argv[i][0] == '-')
        {
            continue;
        }
        else if (strnicmp(argv[i], "COM", 3) == 0)
        {
            comport = i;
        }

        else return;
    }
    return;
}


/*============MAIN==========*/

int main(int argc,char *argv[])
{

    /*handle the program options*/
    HandleOptions(argc,argv);
    if (comport == 0)
    {
        Usage(argv[0]);
        return 0;
    }


    char CFcomport[20] = "\\\\.\\";
    strcat (CFcomport,argv[comport]);

    /* Open COM port */

    HANDLE hMasterCOM = CreateFile(CFcomport,
    GENERIC_READ | GENERIC_WRITE,
    0,
    0,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
    0);

    if (hMasterCOM == INVALID_HANDLE_VALUE)
    {
        int err = GetLastError();
        printf("Error opening port \n");
        return err;
    }
    else
    {
        printf("COM port Opened \n");
    }

    PurgeComm(hMasterCOM, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

    DCB dcbMasterInitState;                     /* Read and save intial state of port */
    GetCommState(hMasterCOM, &dcbMasterInitState);

    Sleep(100);

    /* DTR and RTS are inverted at final output, so CLR means High and SET means Low */

    EscapeCommFunction(hMasterCOM, CLRDTR);     /* start with both lines high */
    EscapeCommFunction(hMasterCOM, CLRRTS);     /* MOSFET Off because GS = 0 */

    timeBeginPeriod(1);                         /* Change system tick resolution from 15ms to 1ms */

    Sleep(50);

    EscapeCommFunction(hMasterCOM, SETDTR);     /* Bring both lines low */
    EscapeCommFunction(hMasterCOM, SETRTS);     /* MOSFET On because capacitor still charged */
    Sleep(pulsetime);                           /* Capacitor discharging, but still high enough */

    EscapeCommFunction(hMasterCOM, CLRRTS);     /* First pulse on RTS - RTS high */
    Sleep(pulsetime);                           /* Capacitor recharging */
    EscapeCommFunction(hMasterCOM, SETRTS);     /* RTS back low */
    Sleep(pulsetime);                           /* Capacitor now discharging, but still high enough */

    EscapeCommFunction(hMasterCOM, CLRRTS);     /* Second pulse on RTS - RTS high */
    Sleep(pulsetime);                           /* Capacitor recharging */
    EscapeCommFunction(hMasterCOM, CLRDTR);     /* DTR back high in the middle of second pulse */
    Sleep(pulsetime);                           /* MOSFET Off because GS = 0 */
    EscapeCommFunction(hMasterCOM, SETRTS);     /* RTS back low */

    Sleep(150);                                 /* Capacitor now fully discharges, and stays that way */
                                                /*   so DTR no longer connected to /Reset */

    timeEndPeriod(1);                           /* Restore system tick */

    dcbMasterInitState.fDtrControl = 0;         /* Change orginal init state DTR to off = high) */
    dcbMasterInitState.fRtsControl = 1;         /* Change orginal init state RTS to on = low) */

    printf("BSL invoke pattern transmitted \n");

    SetCommState(hMasterCOM, &dcbMasterInitState);  /* Restore original init state */

    Sleep(60);                                      /* Sleep a little more */

    CloseHandle(hMasterCOM);                        /* Close port - DTR = high. RTS = low */

    hMasterCOM = INVALID_HANDLE_VALUE;

}
