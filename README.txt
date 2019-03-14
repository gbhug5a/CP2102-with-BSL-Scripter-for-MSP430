
Using a CP2102 with BSL-Scripter for MSP430
-------------------------------------------

UPDATE:  The hardware circuit referred to below works with the CP2102,
and with the FT232 if its COM Port Advanced Settings have "Set RTS
on Close" selected.  The circuit does not work with the CH340G.


This is about using a CP2102 or similar USB-to-Uart adapter as the hardware
interface when flashing MSP430 parts with TI's BSL-Scripter software.

Included is Windows software to generate the BSL invoke pattern on /Reset
and Test, as well as a simple hardware circuit that disconnects DTR from
/Reset when Scripter runs, which allows flashing to proceed.

Everything is explained in the PDF file.
