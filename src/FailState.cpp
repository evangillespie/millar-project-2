#include "Arduino.h"
#include "FailState.h"

/*
	Constructor. Generic. Boring
*/
FailState::FailState() {

}

long FailState::_get_pause_length(int index) {
	long _pause_lengths[] = {
		1000,	// pause before the 0th state - Pause 10
		1000,	// pause before the 1st state - Pause 11
		2000,	// 2nd - Pause 12
		2000,	// 3 - Pause 13
		2000, 	// 4 - Pause 14
		0 		// 5 - no pause
	};
		
	return _pause_lengths[index];
}

int FailState::_get_max_state() {
	return 5;
}


void FailState::_dispatcher() {
	switch(_state_num){
		case 0:
			Serial.println("Fail: Zero");
			// badpin sound
			g_sound_manager.play_sound(1);

			// @TODO: these
			// bargraph off
			// ControlPanel led off
			// 4-digit display off
			// turn off all pindigit leds
			// Turn off Keypad clr and ok leds

			_increment_state();
			break;

		case 1:
			Serial.println("Fail: One");

			// Servo 6 moves

			// Turn off Bricklamp led
			// g_led_flash_manager.stop_flasher(*)	waiting on pin assignment

			// Keypad Green LED fades down over 3s
			g_led_fade_manager.fade(11, 3000, 255, 0);
			
			// Keypad buttons fade down over 3s
			g_led_fade_manager.fade(5, 3000, 255, 0);
			g_led_fade_manager.fade(6, 3000, 255, 0);
			g_led_fade_manager.fade(7, 3000, 255, 0);

			_increment_state();
			break;
		case 2:
			Serial.println("Fail: Two");

			// Keypad yellow leds fade down over 5s
			g_led_fade_manager.fade(1, 5000, 255, 0);
	
			//Servo 5 moves

			//Brick warning finger flashes @ 5hz. blick warning sound triggers on high
			g_led_flash_manager.start_flasher_with_sound(3, 5, 1);

			_increment_state();
			break;
		case 3:
			Serial.println("Fail: Three");

			// Servo 1 moves
			// Servo 2 moves
			// Servo 3 moves
			// Servo 4 moves

			// Keypad door sound
			g_sound_manager.play_sound(1);

			_increment_state();
			break;
		case 4:
			Serial.println("Fail: Four");
			
			//turn off brick warning led
			g_led_flash_manager.stop_flasher(3);
			
			// turn off timer led
			// g_led_flash_manager.stop_flasher(*); wating on pin assignment

			// Servo 8 moves
			// wait until servo 8 stops moving

			_increment_state();
			_substate = 0;
			break;
		case 5:
			if (_substate == 0){
				Serial.println("Fail: Five");
				
				// move servo 7

				_stored_time = millis();
				for (int i=0; i < 4; i++){
					_times[i] = _stored_time + (unsigned long)random(1000, 15000);
					_elements_turned_off[i] = false;
				}
				_substate++;
			} else if (_substate == 1){
				for (int i=0; i < 4; i++){
					if (_elements_turned_off[i] == false){
						if (millis() >= _times[i]){
							switch (i){
								case 0:
									// turn off nixie tube 1
									break;
								case 1:
									// turn off nixie tube 2
									break;
								case 2:
									// turn off led matrix
									break;
								case 3:
									// turn off clock led
									break;
							}
						}
					}
				}
				
				// return to avoid _increment_state() if any of the elemts hasnt been set
				for (int i=0; i < 4; i++){
					if (_elements_turned_off[i] == false){
						return;
					}
				}
				_substate++;

			} else {
				_increment_state();
			}

			break;
		default:
			Serial.println("You're in a weird state, brah");
	}
}