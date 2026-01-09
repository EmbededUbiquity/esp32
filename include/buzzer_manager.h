#ifndef BUZZER_MANAGER_H
#define BUZZER_MANAGER_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Buzzer Pin
#define BUZZER_PIN GPIO_NUM_33

  // Note Frequencies
#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_D5 587
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_G5 784
#define NOTE_A5 880
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_E6 1319
#define NOTE_G6 1568
#define NOTE_C7 2093

  /**
   * Initialize buzzer PWM
   * @return ESP_OK on success
   */
  esp_err_t buzzer_init(void);

  /**
   * Play a specific tone
   * @param freq_hz Frequency in Hz
   * @param duration_ms Duration in ms
   */
  void buzzer_tone(uint32_t freq_hz, uint32_t duration_ms);

  /**
 * Play Game Start Sound
 * Ascending chime indicating game beginning
 */
  void buzzer_play_game_start(void);

  /**
 * Play Game Finish Sound
 * Victory fanfare
 */
  void buzzer_play_game_finish(void);

  /**
 * Play Minigame Start Sound
 * Short, energetic alert
 */
  void buzzer_play_minigame_start(void);

  /**
 * Play Minigame Finish Sound
 * Success chime
 */
  void buzzer_play_minigame_finish(void);

  /**
 * Play Dice Roll Sound
 * Mimics ruffling/rolling sound
 */
  void buzzer_play_dice_roll(void);

  /**
 * Play Damage Sound
 * Low frequency discordant sound
 */
  void buzzer_play_damage(void);

  /**
   * Play Move Sound
   * Short, sharp chime
   */
  void buzzer_play_move(void);

  /**
   * Play Error Sound
   * Low discordant buzz
   */
  void buzzer_play_error(void);

  /**
   * Play Waiting Tune
   * Short pleasant melody
   */
  void buzzer_play_waiting(void);

  /**
 * Play Countdown Sound
 * 3 short beeps followed by a high pitch go signal
 */
  void buzzer_play_countdown(void);

  /**
    * Play Reaction Signal
    * Sharp high pitch ping
    */
  void buzzer_play_reaction_signal(void);

  /**
 * Play Tone for Player 1 (Low Chirp)
 */
  void buzzer_play_tone_player_1(void);

  /**
 * Play Tone for Player 2 (Double Beep)
 */
  void buzzer_play_tone_player_2(void);

  /**
 * Play Tone for Player 3 (Triple High)
 */
  void buzzer_play_tone_player_3(void);

  /**
 * Stop any currently playing sound
 */
  void buzzer_stop(void);

  /**
 * Play a specific tone (frequency in Hz, duration in ms)
 */
  void buzzer_tone(uint32_t freq_hz, uint32_t duration_ms);

#ifdef __cplusplus
}
#endif

#endif // BUZZER_MANAGER_H
