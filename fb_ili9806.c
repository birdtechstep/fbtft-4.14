/*
 * FB driver for the ILI9806 LCD Controller
 *
 * Copyright (C) 2014 Noralf Tronnes
 * Copyright (C) 2019 BIRD TECHSTEP
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
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include "fbtft.h"

#define DRVNAME		"fb_ili9806"
#define WIDTH		480
#define HEIGHT		854
#define BPP			16
#define FPS			30
#define TXBUFLEN	819840 // width*height*2

/* this init sequence matches PiScreen */
static const s16 default_init_sequence[] = {
	/* EXTC Command Set enable register */
	-1, 0xFF, 0xFF, 0x98, 0x06,
	/* SPI Interface Setting */
	-1, 0xBA, 0xE0,
	/* GIP 1 */
	-1, 0xBC, 0x03, 0x0F, 0x63, 0x69, 0x01, 0x01, 0x1B, 0x11, 0x70, 0x73, 0xFF, 0xFF, 0x08, 0x09, 0x05, 0x00, 0xEE, 0xE2, 0x01, 0x00, 0xC,
	/* GIP 2 */
	-1, 0xBD, 0x01, 0x23, 0x45, 0x67, 0x01, 0x23, 0x45, 0x67,
	/* GIP 3 */
	-1, 0xBE, 0x00, 0x22, 0x27, 0x6A, 0xBC, 0xD8, 0x92, 0x22, 0x22,
	/* Vcom */
	-1, 0xC7, 0x1E,
 	/* EN_volt_reg */
	-1, 0xED, 0x7F, 0x0F, 0x00,
	/* Power Control 1 */
	-1, 0xC0, 0xE3, 0x0B, 0x00,
	-1, 0xFC, 0x08,
	/* Engineering Setting */
	-1, 0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
	/* DVDD Voltage Setting */
	-1, 0xF3, 0x74,
	/* Display Inversion Control */
	-1, 0xB4, 0x00, 0x00, 0x00,
	/* 480x854 */
	-1, 0xF7, 0x81,
	/* Frame Rate */
	-1, 0xB1, 0x00, 0x10, 0x14,
	/* Panel Timing Control */
	-1, 0xF1, 0x29, 0x8A, 0x07,
	/* Panel Timing Control */
	-1, 0xF2, 0x40, 0xD2, 0x50, 0x28,
	/* Power Control 2 */
	-1, 0xC1, 0x17, 0X85, 0x85, 0x20,
	-1, 0xE0, 0x00, 0x0C, 0x15, 0x0D, 0x0F, 0x0C, 0x07, 0x05, 0x07, 0x0B, 0x10, 0x10, 0x0D, 0x17, 0x0F, 0x00,
	-1, 0xE1, 0x00, 0x0D, 0x15, 0x0E, 0x10, 0x0D, 0x08, 0x06, 0x07, 0x0C, 0x11, 0x11, 0x0E, 0x17, 0x0F, 0x00,
	/* Tearing Effect ON */
	-1, 0x35, 0x00,
	-1, 0x36, 0x00,
	-1, 0x3A, 0x55,
	/* SPI Interface Setting */
	-1, 0xB6, 0xA2,
	/* Exit Sleep */
	-1, 0x11,
	-2, 120,
	/* Display On */
	-1, 0x29,
	-2, 25,
	/* end marker */
	-3
};

static void set_addr_win(struct fbtft_par *par, int xs, int ys, int xe, int ye)
{
	fbtft_par_dbg(DEBUG_SET_ADDR_WIN, par,
		"%s(xs=%d, ys=%d, xe=%d, ye=%d)\n", __func__, xs, ys, xe, ye);

	/* Column address */
	write_reg(par, 0x2A, xs >> 8, xs & 0xFF, xe >> 8, xe & 0xFF);

	/* Row adress */
	write_reg(par, 0x2B, ys >> 8, ys & 0xFF, ye >> 8, ye & 0xFF);

	/* Memory write */
	write_reg(par, 0x2C);
}

static int set_var(struct fbtft_par *par)
{
	fbtft_par_dbg(DEBUG_INIT_DISPLAY, par, "%s()\n", __func__);

	switch (par->info->var.rotate) {
	case 0:
		write_reg(par, 0x36, 0x40 | (par->bgr << 3));
		break;
	case 90:
		write_reg(par, 0x36, 0x60 | (par->bgr << 3));
		break;
	case 180:
		write_reg(par, 0x36, 0xC0 | (par->bgr << 3));
		break;
	case 270:
		write_reg(par, 0x36, 0xA0 | (par->bgr << 3));
		break;
	default:
		break;
	}

	return 0;
}

static struct fbtft_display display = {
	.regwidth = 8,
	.width = WIDTH,
	.height = HEIGHT,
	.txbuflen = TXBUFLEN,
	.bpp = BPP,
	.fps = FPS,
	.init_sequence = default_init_sequence,
	.fbtftops = {
		.set_addr_win = set_addr_win,
		.set_var = set_var,
	},
};

FBTFT_REGISTER_DRIVER(DRVNAME, "ilitek,ili9806", &display);

MODULE_ALIAS("spi:" DRVNAME);
MODULE_ALIAS("platform:" DRVNAME);
MODULE_ALIAS("spi:ili9806");
MODULE_ALIAS("platform:ili9806");

MODULE_DESCRIPTION("FB driver for the ILI9806 LCD Controller");
MODULE_AUTHOR("BIRD TECHSTEP");
MODULE_LICENSE("GPL");
