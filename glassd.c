#include <linux/input-event-codes.h>
#include <errno.h>

#include <stdio.h>
#include <unistd.h>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>

#define DEVICE_DESCRIPTION_END -1

//utility function for registering all evdev input event codes sent by our virtual input devices.
//array of input codes must be terminated by {-1, -1}.
//this is similar in practice to HID's device descriptors
void register_input_codes(struct libevdev *handle, int codes[]){
    for (int i=0; codes[i] != -1 && codes[i+1] != -1; i+=2){
        libevdev_enable_event_code(handle, codes[i], codes[i+1], NULL);
    }
}

void press_F_to_pay_respects(struct libevdev_uinput *handle){
    libevdev_uinput_write_event(handle, EV_KEY, KEY_F, 1);
    libevdev_uinput_write_event(handle, EV_SYN, SYN_REPORT, 0);
    libevdev_uinput_write_event(handle, EV_KEY, KEY_F, 0);
    libevdev_uinput_write_event(handle, EV_SYN, SYN_REPORT, 0);
}

int main(){
    struct libevdev *virtual_keyboard_template = libevdev_new();
    
    //virtual keyboard definition
    libevdev_set_name(virtual_keyboard_template, "glassd virtual keyboard");
    int virtual_keyboard_events_definition[] = {
        EV_SYN, SYN_REPORT,
        EV_KEY, KEY_F,
        EV_KEY, KEY_F10,
        EV_KEY, KEY_TAB,
        EV_KEY, KEY_ESC,
        EV_KEY, KEY_SPACE,
        DEVICE_DESCRIPTION_END, DEVICE_DESCRIPTION_END
    };
    register_input_codes(virtual_keyboard_template, virtual_keyboard_events_definition);
    
    //virtual keyboard creation
    struct libevdev_uinput *virtual_keyboard_device = NULL;
    libevdev_uinput_create_from_device(virtual_keyboard_template, LIBEVDEV_UINPUT_OPEN_MANAGED, &virtual_keyboard_device);
    
    while(1){
        press_F_to_pay_respects(virtual_keyboard_device);
        sleep(1);
    }
    
}
