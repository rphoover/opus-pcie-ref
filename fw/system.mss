
 PARAMETER VERSION = 2.2.0


BEGIN OS
 PARAMETER OS_NAME = standalone
 PARAMETER OS_VER = 3.07.a
 PARAMETER PROC_INSTANCE = ppc440_0
 PARAMETER STDIN = rs232
 PARAMETER STDOUT = rs232
END


BEGIN PROCESSOR
 PARAMETER DRIVER_NAME = cpu_ppc440
 PARAMETER DRIVER_VER = 2.01.a
 PARAMETER HW_INSTANCE = ppc440_0
END


BEGIN DRIVER
 PARAMETER DRIVER_NAME = pcie
 PARAMETER DRIVER_VER = 4.01.a
 PARAMETER HW_INSTANCE = plbv46_pcie_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = uartlite
 PARAMETER DRIVER_VER = 2.00.a
 PARAMETER HW_INSTANCE = rs232
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = bram
 PARAMETER DRIVER_VER = 3.01.a
 PARAMETER HW_INSTANCE = xps_bram_if_cntlr_1
END


