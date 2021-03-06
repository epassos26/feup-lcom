#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"
#include "timer.h"
#include "read_xpm.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	0xF0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8

/* Private global variables */

static char *video_mem; /* Process address to which VRAM is mapped */
char* double_buffer;

static unsigned h_res; /* Horizontal screen resolution in pixels */
static unsigned v_res; /* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

void *vg_init(unsigned short mode) {
	vbe_mode_info_t info;
	struct reg86u reg86;
	int r;
	struct mem_range mr;

	reg86.u.b.intno = BIOS_VIDEO_INT; /* BIOS video services */
	reg86.u.w.ax = SET_VBE_MODE; /* Set Video Mode function */
	reg86.u.w.bx = SET_LINEAR_MODE | mode; /* Mode */

	if (sys_int86(&reg86) != OK) { // Sets video mode
		printf("\tvg_init(): sys_int86() failed \n");
		return NULL;
	}

	switch (reg86.u.w.ax) {
	case VBE_FUNC_CALL_FAILED:
		printf("\tvg_init(): sys_int86() function call failed.\n");
		return NULL;
		break;
	case VBE_FUNC_NOT_SUPPORTED:
		printf("\tvg_init(): sys_int86() function not supported.\n");
		return NULL;
		break;
	case VBE_FUNC_INVALID_CUR_MODE:
		printf(
				"\tvg_init(): sys_int86() function invalid in current video mode.\n");
		return NULL;
		break;
	}

	if (lm_init() == NULL) {
		printf("\tvg_init(): lm_init() failed \n");
		return NULL;
	}

	if (vbe_get_mode_info(mode, &info) != OK) { // Gets info
		printf("\tvg_init(): vbe_get_mode_info() failed \n");
		return NULL;
	}

	h_res = info.XResolution;
	v_res = info.YResolution; //Sets global variables
	bits_per_pixel = info.BitsPerPixel;

	//Allow memory mapping

	mr.mr_base = (phys_bytes)(info.PhysBasePtr);
	mr.mr_limit = mr.mr_base
			+ info.XResolution * info.YResolution * info.BitsPerPixel / 8;

	if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("video_txt: sys_privctl (ADD_MEM) failed: %d\n", r);

	// Map memory

	video_mem = vm_map_phys(SELF, (void *) mr.mr_base,
			info.XResolution * info.YResolution * info.BitsPerPixel / 8);

	double_buffer = malloc(h_res * v_res * sizeof(char));

	/*if(video_mem == MAP_FAILED)
	 panic("video_txt couldn't map video memory");*/

	return video_mem;
}

int vg_set_pixel(unsigned short x, unsigned short y, unsigned long color) {
	int ret = 0;

	if (x < 0) {
		x = 0;
		ret = 1;
	} else if (x >= h_res) {
		x = h_res - 1;
		ret = 2;
	}

	if (y < 0) {
		y = 0;
		ret = 3;
	} else if (y >= v_res) {
		y = v_res - 1;
		ret = 4;
	}

	*(double_buffer + y * h_res + x) = color;

	return ret;
}

int vg_draw_frame(unsigned short x, unsigned short y, unsigned short width,
		unsigned short height, unsigned long color) {
	char* vmem = double_buffer;
	vmem += y * h_res + x;

	unsigned int i;

	//Prints first line
	for (i = 0; i < width; i++) {
		_vg_set_pixel(x+i, y, color);
		//vg_set_pixel(x + i, y, color);
	}

	//Prints vertical lines
	for (i = 1; i < height - 1; i++) {
		vg_set_pixel(x, y + i, color);
		vg_set_pixel(x + width, y + i, color);
	}

	//Prints last line
	for (i = 0; i < width; i++) {
		vg_set_pixel(x + i, y + height - 1, color);
	}

	return 0;
}

int vg_draw_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color) {
	char* vmem = double_buffer;

	if (xi == xf) {
		unsigned int i;
		for (i = 0; i < yf - yi; i++) {
			vg_set_pixel(xi, yi + i, color);
		}
	} else if (yi == yf) {
		unsigned int i;
		for (i = 0; i < xf - xi; i++) {
			vg_set_pixel(xi + i, yi, color);
		}
	} else {
		unsigned int offset = 0;
		float m = ((double) (yf - yi)) / (xf - xi);
		if (m < 0)
			offset = 1;

		if (m > -1 && m < 1) {
			int i;
			for (i = xi; i <= xf; i++) {
				vg_set_pixel(i, offset * yi + (int) (m * i), color);
			}
		} else {
			int i;
			for (i = yi; i <= yf; i++) {
				vg_set_pixel(offset * xi + (int) (i / m), i, color);
			}
		}
	}

	return 0;
}

char vg_draw_pixmap(unsigned short *pixmap, unsigned short xi, unsigned short yi, unsigned short width, unsigned short height) {
	if (xi + width >= h_res || yi + height >= v_res || xi < 0 || yi < 0)
		return 1;

	unsigned short i, j;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			vg_set_pixel(xi + j, yi + i, *(pixmap + i * height + j));
		}
	}

	return 0;
}

char vg_draw_xpm(unsigned short xi, unsigned short yi, char *xpm[]) {
	int width, height;
	char *pixmap;

	if ((pixmap = read_xpm(xpm, &width, &height)) == NULL)
		return 1;

	char ret = vg_draw_pixmap(xi, yi, (unsigned short) width,
			(unsigned short) height, pixmap);
	free(pixmap);
	return ret;
}

int vg_move_pixmap(unsigned short xi, unsigned short yi, unsigned short width,
		unsigned short height, char *pixmap, unsigned short hor,
		float next_position) {

	if (!hor) {
		if (yi + height + (short) next_position >= (short) v_res) {
			vg_draw_pixmap(xi, v_res - height - 1, width, height, pixmap);
			return 1;
		}

		if (yi + (short) next_position < 0) {
			vg_draw_pixmap(xi, 0, width, height, pixmap);
			return 1;
		}

		vg_draw_pixmap(xi, yi + (short) next_position, width, height, pixmap);
	} else {
		if (xi + width + (short) next_position >= (short) h_res) {
			vg_draw_pixmap(h_res - width - 1, width, height, yi, pixmap);
			return 1;
		}

		if (xi + (short) next_position < 0) {
			vg_draw_pixmap(0, yi, width, height, pixmap);
			return 1;
		}

		vg_draw_pixmap(xi + (short) next_position, yi, width, height, pixmap);
	}

	return 0;
}

void vg_clear_screen() {
	unsigned short i, j;
	for (i = 0; i < v_res; i++) {
		for (j = 0; j < h_res; j++) {
			*(double_buffer + i * h_res + j) = 0;
		}
	}
}

int vg_update_screen() {
	if (double_buffer == NULL)
		return 1;

	memcpy(video_mem, double_buffer, h_res * v_res);

	vg_clear_screen();

	return 0;
}


int vg_exit() {
	struct reg86u reg86;

	free(double_buffer);

	reg86.u.b.intno = 0x10; /* BIOS video services */

	reg86.u.b.ah = 0x00; /* Set Video Mode function */
	reg86.u.b.al = 0x03; /* 80x25 text mode*/

	if (sys_int86(&reg86) != OK) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}
