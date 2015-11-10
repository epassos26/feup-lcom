#include "timer.h"
#include "test4.h"
#include "timer.h"
#include "i8254.h"
#include <minix/syslib.h>
#include <minix/drivers.h>

unsigned int count = 0;

int test_packet(unsigned short cnt) {
	int irq_set_mouse = mouse_subscribe_int();
	int r;
	int ipc_status;
	char packet[3];
	message msg;

	if (irq_set_mouse >= 0)
		irq_set_mouse = BIT(irq_set_mouse);
	else
		irq_set_mouse = 0;

	unsigned int i;
	for (i = 0; i < cnt; i++) {
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set_mouse) { /* subscribed interrupt */
					count++;
					mouse_int_handler(count, packet);
					printf("Byte %d: %x\t", count, packet[count - 1]);
					if (count == 3) {
						printf("\n");
						count = 0;
					}
					break;
			default:
				break; /* no other notifications expected: do nothing */
				} else { /* received a standard message, not a notification */
					/* no standard messages expected: do nothing */
				}
			}
		}
	}
}

int test_async(unsigned short idle_time) {
	int irq_set_mouse = mouse_subscribe_int();
	int irq_set_timer = timer_subscribe_int();
	int r;
	int ipc_status;
	unsigned short time = 0;

	char packet[3];
	message msg;

	if (irq_set_mouse >= 0)
		irq_set_mouse = BIT(irq_set_mouse);
	else
		irq_set_mouse = 0;

	while (time < idle_time) {
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set_mouse) { /* subscribed interrupt */
					time = 0;
					count++;
					mouse_int_handler(count, packet);
					printf("Byte %d: %x\t", count, packet[count - 1]);
					if (count == 3) {
						printf("\n");
						count = 0;
					}
				}
				if (msg.NOTIFY_ARG & irq_set_timer) {
					timed_scan_int_handler(&time);
				}

				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
}

int test_config(void) {
	char packet[3];
	char ack;
	int error = 0;
	while (error) {
		error = 0;
		write_to_KBC(KBC_COMMAND, WRITE_TO_MOUSE);
		write_to_KBC(KBC_OUT_BUF, MOUSE_STATUS_REQUEST);
		unsigned int i;
		for (i = 0; i < 3; i++) {
			read_from_KBC(KBC_IN_BUF, &ack);
			if (ack != ACK) {
				error = 1;
				break;
			} else {
				read_from_KBC(KBC_OUT_BUF, &packet[i]);
				printf("Byte %d: %x\t", i, packet[i]);
			}
		}
	}
}

int test_gesture(short length, unsigned short tolerance) {

}