#include <linux/input-event-codes.h>

#include <omniGlass/constants.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>

#include <omniGlass/omniglass.h>

#define GLASSD_DEFAULTS_TOUCHPAD_DEADZONE 0.1
#define GLASSD_DEFAULTS_TAB_MOVEMENT_THRESHOLD 1.6

#define DEVICE_DESCRIPTION_END -1

typedef enum { INHIBIT, MENU, JINX } tracking_mode;

typedef enum { STARTED, DRAGGING, CROSSED } dragging_action_state;

struct glassd_state {
    double movement_deadzone;
    double edge_slide_accumulated;
    double edge_slide_threshold;
    int tapped;
    int finger_pressed;
    tracking_mode current_mode;

    omniglass_raw_specifications *touchpad_specifications;
    omniglass_raw_touchpoint single_finger_last_state;
    omniglass_raw_report *current_report;

};

double euclidean_distance(omniglass_raw_touchpoint p0, omniglass_raw_touchpoint p1){
    return (pow((p1.x - p0.x), 2) + pow((p1.y - p0.y),2))
}

//utility function for registering all evdev input event codes sent by our virtual input devices.
//array of input codes must be terminated by {-1, -1}.
//this is similar in practice to HID's device descriptors
void register_input_codes(struct libevdev *handle, int codes[]){
    for (int i=0; codes[i] != -1 && codes[i+1] != -1; i+=2){
        libevdev_enable_event_code(handle, codes[i], codes[i+1], NULL);
    }
}

//
//SHORTHAND FUNCTIONS: virtual keypresses
//
void press_F_to_pay_respects(struct libevdev_uinput *handle){
    libevdev_uinput_write_event(handle, EV_KEY, KEY_F, 1);
    libevdev_uinput_write_event(handle, EV_SYN, SYN_REPORT, 0);
    libevdev_uinput_write_event(handle, EV_KEY, KEY_F, 0);
    libevdev_uinput_write_event(handle, EV_SYN, SYN_REPORT, 0);
}

void generate_tab(struct libevdev_uinput *handle){
    libevdev_uinput_write_event(handle, EV_KEY, KEY_TAB, 1);
    libevdev_uinput_write_event(handle, EV_SYN, SYN_REPORT, 0);
    libevdev_uinput_write_event(handle, EV_KEY, KEY_TAB, 0);
    libevdev_uinput_write_event(handle, EV_SYN, SYN_REPORT, 0);
}

void generate_shift_tab(struct libevdev_uinput *handle){
    libevdev_uinput_write_event(handle, EV_KEY, KEY_LEFTSHIFT, 1);
    libevdev_uinput_write_event(handle, EV_SYN, SYN_REPORT, 0);
    libevdev_uinput_write_event(handle, EV_KEY, KEY_TAB, 1);
    libevdev_uinput_write_event(handle, EV_SYN, SYN_REPORT, 0);
    libevdev_uinput_write_event(handle, EV_KEY, KEY_TAB, 0);
    libevdev_uinput_write_event(handle, EV_KEY, KEY_LEFTSHIFT, 0);
    libevdev_uinput_write_event(handle, EV_SYN, SYN_REPORT, 0);
}

void glassd_update_edge(double amount, void *data){
    struct glassd_state *state = data;
    if(amount > state->movement_deadzone || amount < (-1 * state->movement_deadzone))
        state->edge_slide_accumulated += amount;
}
void check_points(omniglass_raw_report *report, void *data){
    struct glassd_state *state = data;
    struct omniglass_raw_touchpoint first_point = report->points[0];
    if (!(state->single_finger_last_state.is_touching)
        && (first_point.is_touching)
        && (first_point.x == 0) )
    {


    }

}


int main(){
    
    //set sane initial values for anything glassd passes to the callbacks.
    struct glassd_state state = {
        GLASSD_DEFAULTS_TOUCHPAD_DEADZONE,
        0.0,
        GLASSD_DEFAULTS_TAB_MOVEMENT_THRESHOLD,
        0,
        0,
        MENU,
        NULL,
        {0,0,0},
        NULL
    };
    
    
    //initialize interface to omniGlass touchpad backend
    struct omniglass *touchpad_handle;
    if (omniglass_init(&touchpad_handle) != OMNIGLASS_RESULT_SUCCESS){
        fprintf(stderr, "failed to initialize touchpad gesture engine.");
    }
    omniglass_get_touchpad_specifications(touchpad_handle, &(state.touchpad_specifications));
    omniglass_listen_gesture_edge(touchpad_handle, glassd_update_edge, OMNIGLASS_EDGE_TOP, &state);
    
    struct libevdev *virtual_keyboard_template = libevdev_new();
    
    //virtual keyboard definition
    libevdev_set_name(virtual_keyboard_template, "glassd virtual keyboard");
    int virtual_keyboard_events_definition[] = {
        EV_SYN, SYN_REPORT,
        EV_KEY, KEY_F,
        EV_KEY, KEY_F10,
        EV_KEY, KEY_TAB,
        EV_KEY, KEY_LEFTSHIFT,
        EV_KEY, KEY_ESC,
        EV_KEY, KEY_SPACE,
        DEVICE_DESCRIPTION_END, DEVICE_DESCRIPTION_END
    };
    register_input_codes(virtual_keyboard_template, virtual_keyboard_events_definition);
    
    //virtual keyboard creation
    struct libevdev_uinput *virtual_keyboard_device = NULL;
    libevdev_uinput_create_from_device(virtual_keyboard_template, LIBEVDEV_UINPUT_OPEN_MANAGED, &virtual_keyboard_device);
    
    while(1){
        omniglass_step(touchpad_handle);
        if (state.edge_slide_accumulated > state.edge_slide_threshold){
            generate_tab(virtual_keyboard_device);
            state.edge_slide_accumulated = 0.0;
        }
        if (state.edge_slide_accumulated < (-1 * state.edge_slide_threshold)){
            generate_shift_tab(virtual_keyboard_device);
            state.edge_slide_accumulated = 0.0;
        }
        usleep(4500);
    }
    
}
