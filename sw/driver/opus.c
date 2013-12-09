/*
 * Opus Linux Driver
 * Copyright (C) 2013 Jay Hirata <jhirata@cmlab.biz>, Computer Measurement Laboratory
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/etherdevice.h>
#include <linux/phy.h>
#include <linux/delay.h>
#include <linux/dma-debug.h>

#define MAX_TRIES               100

char opus_driver_name[] = "Opus";

char message[] = "Hello! The PCI Express link is working!\r\n";


static DEFINE_PCI_DEVICE_TABLE(opus_pci_tbl) = {
    { PCI_DEVICE(0x10ee, 0x1234) },
    { 0 }
};
MODULE_DEVICE_TABLE(pci, opus_pci_tbl);


#define UART_STATUS_OFFSET              0x08
#define UART_STATUS_TX_FULL             0x00000008
#define UART_TX_OFFSET                  0x04

static int write_char(struct device *dev, void __iomem *uart, char c)
{
    int tries;
    u32 status;

    tries = MAX_TRIES;
    while (tries > 0) {
        status = ioread32be(uart + UART_STATUS_OFFSET);
        if (status & UART_STATUS_TX_FULL) {
            msleep(1);
            tries--;
            continue;
        }

        iowrite32be(c, uart + UART_TX_OFFSET);
        break;
    }

    if (tries > 0)
        return 0;
    else
        return -1;
}


/**
 * Device initialization.
 *
 */
static int opus_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    int err;
    resource_size_t mmio_start, mmio_len;
    void __iomem *uart;
    int i;
    u32 reg;

    err = pci_enable_device(pdev);
    if (err)
        goto err_enable;

    err = pci_enable_msi(pdev);
    if (err) {
        dev_err(&pdev->dev, "Couldn't enable MSI interrupts\n");
        goto err_msi;
    }

    /* map pci */
    err = pci_request_region_exclusive(pdev, 0, opus_driver_name);
    if (err) {
        dev_err(&pdev->dev, "Request failed\n");
        goto err_bar_request;
    }

    mmio_start = pci_resource_start(pdev, 0);
    mmio_len = pci_resource_len(pdev, 0);
    uart = ioremap_nocache(mmio_start, mmio_len);
    if (!uart) {
        dev_err(&pdev->dev, "Mapping error: %016llx, %llu\n", mmio_start, mmio_len);
        goto err_ioremap;
    }

    pci_set_drvdata(pdev, uart);

    /* Print registers */
    dev_info(&pdev->dev, "Printing UART registers:\n");
    for (i = 0; i < 4; i++) {
        reg = ioread32be(uart + (i * 4));
        dev_info(&pdev->dev, "  Uart[%d]: %08x\n", i, reg);
    }

    pci_set_drvdata(pdev, uart);

    /* Print registers */
    dev_info(&pdev->dev, "Printing UART registers:\n");
    for (i = 0; i < 4; i++) {
        reg = ioread32be(uart + (i * 4));
        dev_info(&pdev->dev, "  Uart[%d]: %08x\n", i, reg);
    }

    
    if (ioread32be(uart + UART_STATUS_OFFSET) == 0xffffffff) {
        dev_info(&pdev->dev, "Not sure why this doesn't work on the second reload, but the write seems to work? Printing 'OK' to the uart\n");
        dev_info(&pdev->dev, "Still trying to write...\n");
        iowrite32be(0x4f, uart + 4);
        iowrite32be(0x4b, uart + 4);
        err = -EIO;
        goto err_print;
    }

    /* Print message */
    dev_info(&pdev->dev, "Printing the following message to the Opus UART: %s\n", message);
    for (i = 0; i < strlen(message); i++) {
        err = write_char(&pdev->dev, uart, message[i]);
        if (err)
            goto err_print;
    }

    return 0;

err_print:
    pci_set_drvdata(pdev, NULL);
    iounmap(uart);
err_ioremap:
    pci_release_region(pdev, 0);
err_bar_request:
    pci_disable_msi(pdev);
err_msi:
    pci_disable_device(pdev);
err_enable:
    return err;
}

static void opus_remove(struct pci_dev *pdev)
{
    void __iomem *uart;

    uart = pci_get_drvdata(pdev);
    iounmap(uart);
    pci_set_drvdata(pdev, NULL);
    pci_release_region(pdev, 0);
    pci_disable_msi(pdev);
    pci_disable_device(pdev);
}

/* PCI Device API Driver */
static struct pci_driver opus_driver = {
    .name     = opus_driver_name,
    .id_table = opus_pci_tbl,
    .probe    = opus_probe,
    .remove   = opus_remove,
};


static int __init opus_driver_init(void)
{
    pr_info("Loading Opus driver\n");
    return pci_register_driver(&opus_driver);
}

static void __exit opus_driver_exit(void)
{
    pr_info("Unloading Opus driver\n");
    pci_unregister_driver(&opus_driver);
}


module_init(opus_driver_init);
module_exit(opus_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jay Hirata");
MODULE_DESCRIPTION("Simple driver to write to Opus card Uart");

