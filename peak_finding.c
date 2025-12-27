#include <stdio.h>

// find the first peak
int findPeak(int *arr, int i, int n) {
 // happening on the way down
  // base case
  // so it will break from all cases on the way up because i + 1 in the 2nd arg
  if (i >= n)
    return -1;

  // ok for it being the first or last bigger then i + 1 or i - 1 respectivly
  int left_ok = (i == 0) || (arr[i] >= arr[i - 1]);
  int right_ok = (i == n - 1) || (arr[i] >= arr[i + 1]);

  if (left_ok && right_ok) {
    return i;
  }

  return findPeak(arr, i + 1, n);
}

int main(void) {

  int arr[] = {19, 21, 42, 12, 44, 11};

  int n = sizeof(arr) / sizeof(arr[0]);

  int idx = findPeak(arr, 0, n);

  if (idx == -1)
    printf("No peak\n");
  else
    printf("Peak at index %d with value %d\n", idx, arr[idx]);

  return 0;
}
