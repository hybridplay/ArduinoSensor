void sensorON_play(){
  // iterate over the notes of the melody:
  for (int i = 0; i < 3; i++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / sensorON_durations[i];
    tone(speaker, sensorON_melody[i], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(speaker);
  }
}

void sensorLowBattery_play(){
  for (int i = 0; i < 10; i++) {
    int noteDuration = 1000 / 32;

    tone(speaker,NOTE_C4, noteDuration);

    delay(noteDuration * 1.30);
    
    noTone(speaker);
  }
}

