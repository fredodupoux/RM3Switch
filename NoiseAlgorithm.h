// --- 2 mode compare algorithm --- //
int digitalMode(int rawIn, int *sensSmoothArray) {
  int j, k, temp, top;
  int total;
  static int i;
  static int sorted[FILTER_SAMPLES];
  boolean done;

  i = (i + 1) % FILTER_SAMPLES;  // increment counter and roll over if necessary
  sensSmoothArray[i] = rawIn;    // input new data into the oldest slot
  //debugging
  if (debug == 1) {
    Terminal.print(" ");
    Terminal.print(rawIn);
  }

  for (j = 0; j < FILTER_SAMPLES; j++) {  // transfer data array into another array for sorting
    sorted[j] = sensSmoothArray[j];
  }

  done = 0;            // flag to know when we're done sorting
  while (done != 1) {  // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (FILTER_SAMPLES - 1); j++) {
      if (sorted[j] > sorted[j + 1]) {  // numbers are out of order - swap
        temp = sorted[j + 1];
        sorted[j + 1] = sorted[j];
        sorted[j] = temp;

        done = 0;
      }
    }
  }

  // Determine the mode of the highest values
  top = min(FILTER_SAMPLES, (FILTER_SAMPLES - 1));  // Consider all values
  int mode1 = 0;                                    // Initialize mode 1
  int modeCount1 = 0;                               // Initialize mode 1 count

  int mode2 = 0;       // Initialize mode 2
  int modeCount2 = 0;  // Initialize mode 2 count

  for (j = 0; j < top; j++) {
    int currentCount = 1;

    // Check how many times the current value repeats
    for (k = j + 1; k < top; k++) {
      if (sorted[k] == sorted[j]) {
        currentCount++;
      } else {
        break;
      }
    }

    // Update mode 1 and mode 1 count if a new mode is found
    if (currentCount > modeCount1) {
      modeCount1 = currentCount;
      mode1 = sorted[j];
    } else if (currentCount > modeCount2) {
      // Update mode 2 and mode 2 count if a new mode is found
      modeCount2 = currentCount;
      mode2 = sorted[j];
    }

    // Skip repeated values
    j = k - 1;
  }

  // Return the higher mode between mode1 and mode2
  return (mode1 > mode2) ? mode1 : mode2;
}

// -- median algorithm
int digitalSmooth(int rawIn, int *sensSmoothArray) {
  int j, k, temp, top, bottom;
  int total;
  static int i;
  // static int raw[FILTER_SAMPLES];
  static int sorted[FILTER_SAMPLES];
  boolean done;

  i = (i + 1) % FILTER_SAMPLES;  // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
  sensSmoothArray[i] = rawIn;    // input new data into the oldest slot

  for (j = 0; j < FILTER_SAMPLES; j++) {  // transfer data array into another array for sorting and averaging
    sorted[j] = sensSmoothArray[j];
  }

  done = 0;            // flag to know when we're done sorting
  while (done != 1) {  // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (FILTER_SAMPLES - 1); j++) {
      if (sorted[j] > sorted[j + 1]) {  // numbers are out of order - swap
        temp = sorted[j + 1];
        sorted[j + 1] = sorted[j];
        sorted[j] = temp;
        done = 0;
      }
    }
  }
  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((FILTER_SAMPLES * 15) / 100), 1);
  top = min((((FILTER_SAMPLES * 85) / 100) + 1), (FILTER_SAMPLES - 1));  // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for (j = bottom; j < top; j++) {
    total += sorted[j];  // total remaining indices
    k++;
  }
  return total / k;  // divide by number of samples
}