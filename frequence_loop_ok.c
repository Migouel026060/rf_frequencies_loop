#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <lib/rf/rf.h>
#include <lib/toolbox/random.h>

#define MIN_BITS 8
#define MAX_BITS 52

const uint32_t frequencies[] = {
    300000000,
    315000000,
    360000000,
    390000000,
    433920000
};
const size_t freq_count = sizeof(frequencies) / sizeof(uint32_t);

volatile bool is_running = false;

// Callback bouton : bascule on/off avec OK
void input_callback(InputEvent* event, void* ctx) {
    if(event->type == InputTypePress && event->key == InputKeyOk) {
        is_running = !is_running;
    }
}

// Affichage
void draw_ui(Canvas* canvas, void* ctx) {
    uint32_t* state = (uint32_t*)ctx;
    uint32_t freq = state[0];
    uint8_t bits = (uint8_t)state[1];

    canvas_clear(canvas);
    canvas_set_font(canvas, FontSecondary);

    canvas_draw_str(canvas, 0, 10, is_running ? "Émission: ON" : "Émission: OFF");

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Freq: %.2f MHz", freq / 1e6);
    canvas_draw_str(canvas, 0, 25, buffer);
    snprintf(buffer, sizeof(buffer), "Bits: %u", bits);
    canvas_draw_str(canvas, 0, 40, buffer);
}

int32_t frequence_loop_app(void* p) {
    UNUSED(p);

    // Init
    RfTx* rf_tx = rf_tx_alloc();
    ViewPort* view_port = view_port_alloc();
    Gui* gui = furi_record_open("gui");

    uint32_t draw_state[2] = {0};  // [0]=freq, [1]=bitlen
    view_port_draw_callback_set(view_port, draw_ui, draw_state);
    view_port_input_callback_set(view_port, input_callback, NULL);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    while(1) {
        if(is_running) {
            for(size_t i = 0; i < freq_count && is_running; i++) {
                rf_tx_set_frequency(rf_tx, frequencies[i]);
                draw_state[0] = frequencies[i];

                for(uint8_t bitlen = MIN_BITS; bitlen <= MAX_BITS && is_running; bitlen++) {
                    draw_state[1] = bitlen;

                    uint64_t data = random_uint64() >> (64 - bitlen);
                    rf_tx_set_modulation(rf_tx, RfTxModulationASK_OOK);
                    rf_tx_set_bit_length(rf_tx, bitlen);
                    rf_tx_set_data(rf_tx, data);
                    rf_tx_transmit(rf_tx);

                    view_port_update(view_port);
                    furi_delay_ms(20);
                }

                if(is_running) {
                    furi_delay_ms(10000); // 10 secondes entre les fréquences
                }
            }
        } else {
            view_port_update(view_port);
            furi_delay_ms(100);
        }
    }

    // Clean up (jamais atteint sauf interruption)
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close("gui");
    rf_tx_free(rf_tx);

    return 0;
}
