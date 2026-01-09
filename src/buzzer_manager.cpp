#include "buzzer_manager.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "BUZZER";

// LEDC Configuration
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO (33)
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define LEDC_DUTY (4096)
#define LEDC_FREQUENCY (5000)

esp_err_t buzzer_init(void)
{
    ESP_LOGI(TAG, "Initializing buzzer on GPIO %d", LEDC_OUTPUT_IO);

    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_DUTY_RES,
        .timer_num = LEDC_TIMER,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .gpio_num = LEDC_OUTPUT_IO,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER,
        .duty = 0,
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    return ESP_OK;
}

void buzzer_tone(uint32_t freq_hz, uint32_t duration_ms)
{
    if (freq_hz == 0)
    {
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        vTaskDelay(pdMS_TO_TICKS(duration_ms));
        return;
    }

    ledc_set_freq(LEDC_MODE, LEDC_TIMER, freq_hz);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
    vTaskDelay(pdMS_TO_TICKS(duration_ms));
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

void buzzer_stop(void)
{
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

// --- Sound Effects ---

void buzzer_play_move(void)
{
    buzzer_tone(800, 100);
}

void buzzer_play_game_start(void)
{
    // Rising Nintendo-style startup
    buzzer_tone(NOTE_E5, 100);
    buzzer_tone(NOTE_B5, 100);
    buzzer_tone(NOTE_C6, 100);
    buzzer_tone(NOTE_G6, 200);
    buzzer_tone(0, 50);
    buzzer_tone(NOTE_G6, 300);
}

void buzzer_play_game_finish(void)
{
    // Final Fantasy Victory Fanfare (abbreviated)
    buzzer_tone(NOTE_C5, 100);
    buzzer_tone(NOTE_C5, 100);
    buzzer_tone(NOTE_C5, 100);
    buzzer_tone(NOTE_C5, 300);
    buzzer_tone(NOTE_G4, 300);
    buzzer_tone(NOTE_A4, 300);
    buzzer_tone(NOTE_C5, 200);
    buzzer_tone(NOTE_A4, 100);
    buzzer_tone(NOTE_C5, 400);
}

void buzzer_play_minigame_start(void)
{
    // Quick double chirp
    buzzer_tone(NOTE_A5, 80);
    buzzer_tone(0, 20);
    buzzer_tone(NOTE_E6, 150);
}

void buzzer_play_minigame_finish(void)
{
    // Success triad
    buzzer_tone(NOTE_E5, 100);
    buzzer_tone(NOTE_G5, 100);
    buzzer_tone(NOTE_C6, 200);
}

void buzzer_play_dice_roll(void)
{
    // Randomized "clicking" sound slowing down
    int delay = 20;
    for (int i = 0; i < 15; i++)
    {
        buzzer_tone(200 + (esp_random() % 500), 10);
        vTaskDelay(pdMS_TO_TICKS(delay));
        delay += 10;
    }
    // Final "result" ding
    buzzer_tone(NOTE_C6, 200);
}

void buzzer_play_damage(void)
{
    // Discordant drop
    buzzer_tone(150, 100);
    buzzer_tone(100, 200);
}

void buzzer_play_countdown(void)
{
    buzzer_tone(NOTE_C5, 100); // 3
    vTaskDelay(pdMS_TO_TICKS(900));

    buzzer_tone(NOTE_C5, 100); // 2
    vTaskDelay(pdMS_TO_TICKS(900));

    buzzer_tone(NOTE_C5, 100); // 1
    vTaskDelay(pdMS_TO_TICKS(900));

    buzzer_tone(NOTE_C6, 500); // GO!
}

void buzzer_play_reaction_signal(void)
{
    buzzer_tone(NOTE_C7, 150);
}

void buzzer_play_error(void)
{
    // Error buzz
    buzzer_tone(100, 150);
    buzzer_tone(0, 50);
    buzzer_tone(100, 150);
}

void buzzer_play_waiting(void)
{
    // Measure 1
    buzzer_tone(NOTE_G4, 150);
    buzzer_tone(NOTE_C5, 150);
    buzzer_tone(NOTE_E5, 150);
    buzzer_tone(NOTE_G5, 150);
    buzzer_tone(NOTE_E5, 150);
    buzzer_tone(NOTE_C5, 150);

    buzzer_tone(0, 50);

    // End
    buzzer_tone(NOTE_G4, 300);
}

void buzzer_play_tone_player_1(void)
{
    // Total: 160ms
    buzzer_tone(NOTE_C5, 160);
}

void buzzer_play_tone_player_2(void)
{
    // Standardized to C5 160ms
    buzzer_tone(NOTE_C5, 160);
}

void buzzer_play_tone_player_3(void)
{
    ESP_LOGI(TAG, "Playing Player 3 Tone");
    // Standardized to C5 160ms
    buzzer_tone(NOTE_C5, 160);
}
