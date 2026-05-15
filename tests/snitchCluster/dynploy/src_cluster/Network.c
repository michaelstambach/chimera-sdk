#include "DeeploySnitchMath.h"
#include "snrt.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "Network.h"

int8_t *DeeployNetwork_MEMORYARENA_L1;
int8_t *DeeployNetwork_MEMORYARENA_L2;
int8_t *DeeployNetwork_input_0;
int32_t *DeeployNetwork_output_0;

static int8_t DeeployNetwork_weight_tensor[1024] = {
    43,   29,   15,   12,   78,   -19,  67,   -91,  54,   -34,  19,   -121, -99,  -100, 31,   15,   -100, -29,  115,  59,   77,   -116, 39,   113,  -93,  15,
    15,   99,   -64,  104,  -67,  14,   -10,  -119, 63,   102,  33,   -69,  86,   -17,  -32,  2,    -16,  51,   -50,  -70,  22,   -100, 65,   -87,  -107, -42,
    37,   70,   106,  64,   -8,   10,   44,   -80,  -12,  -9,   42,   115,  0,    -13,  101,  7,    4,    -16,  -68,  63,   -28,  -45,  -117, -26,  -117, 85,
    -64,  -57,  -79,  121,  -34,  -33,  47,   -4,   -54,  -127, -109, 19,   -45,  -29,  20,   19,   119,  48,   100,  72,   -91,  52,   16,   37,   101,  26,
    -46,  3,    97,   121,  73,   -50,  101,  -101, 49,   -25,  26,   -120, -67,  115,  -99,  36,   16,   -51,  -9,   89,   -32,  15,   16,   -8,   -19,  5,
    -69,  -48,  100,  -60,  122,  89,   -41,  -51,  -103, 51,   -117, -17,  -97,  88,   -41,  27,   15,   -72,  -25,  36,   113,  -118, -105, 103,  -73,  -59,
    98,   -57,  -69,  -3,   -23,  -12,  -15,  29,   -57,  53,   -35,  -39,  21,   -117, -31,  29,   -112, -63,  -49,  -71,  -71,  123,  112,  -99,  50,   -29,
    29,   -22,  12,   -72,  -30,  65,   -21,  108,  -111, -72,  66,   53,   -33,  -128, -66,  -115, 41,   6,    126,  13,   -33,  55,   -68,  -52,  -45,  -70,
    80,   -80,  -111, 28,   -60,  -26,  61,   -96,  -29,  101,  27,   -29,  5,    -99,  68,   65,   97,   107,  -32,  87,   -105, -27,  -88,  88,   -126, 100,
    -83,  -15,  -5,   -117, -113, 88,   -87,  -1,   -118, 95,   110,  -92,  -70,  126,  -19,  103,  -83,  -57,  -43,  35,   -68,  -24,  12,   65,   112,  79,
    -37,  -109, 60,   119,  -120, -47,  -122, 11,   -25,  -103, -77,  76,   90,   10,   17,   34,   76,   -98,  -104, -100, 56,   18,   98,   -127, -104, 97,
    25,   7,    -113, -85,  80,   -54,  -18,  -21,  81,   79,   -118, 26,   -39,  67,   63,   -25,  121,  -115, 112,  -60,  3,    58,   57,   -14,  -14,  58,
    66,   71,   -47,  1,    -126, -80,  14,   69,   -10,  -49,  -96,  124,  -121, -120, 51,   -27,  -123, 70,   -40,  77,   59,   -117, 12,   18,   66,   -111,
    103,  -1,   -9,   19,   -112, 66,   -108, -68,  87,   71,   125,  49,   -19,  15,   -95,  -66,  -66,  3,    125,  -32,  -88,  -87,  -90,  47,   109,  -6,
    1,    -38,  45,   -50,  47,   -125, -55,  -2,   101,  38,   64,   92,   3,    -104, 27,   25,   125,  -48,  -67,  -9,   23,   -52,  10,   -67,  -106, 81,
    -69,  17,   38,   -30,  -90,  -73,  18,   109,  -118, 78,   -111, -27,  -99,  81,   112,  44,   -28,  -63,  -47,  32,   -51,  -28,  71,   56,   53,   105,
    12,   -81,  12,   -61,  -30,  69,   106,  80,   -28,  -107, -77,  -40,  53,   34,   116,  -108, -108, -34,  67,   92,   4,    -93,  98,   77,   44,   -20,
    48,   -51,  -20,  -65,  -47,  -76,  5,    -52,  -35,  16,   30,   -4,   94,   -16,  65,   20,   -112, 123,  -100, 1,    -23,  -57,  38,   87,   71,   -17,
    -23,  -114, -5,   -45,  -90,  -89,  87,   -20,  75,   88,   -99,  -96,  -90,  21,   -64,  -16,  118,  -28,  -99,  -85,  -11,  42,   23,   124,  64,   126,
    -48,  55,   -64,  -99,  80,   16,   -92,  -99,  -115, -81,  -86,  -12,  -34,  50,   115,  58,   70,   -41,  -127, -8,   29,   73,   -48,  1,    97,   -18,
    -69,  -6,   90,   120,  -43,  16,   -58,  -84,  6,    -26,  98,   72,   -48,  51,   -127, -47,  74,   -24,  5,    27,   -100, 5,    -100, 78,   -118, -110,
    -38,  -44,  27,   125,  -34,  69,   36,   57,   -125, -60,  88,   -127, -107, 107,  108,  -5,   4,    -69,  82,   -68,  123,  36,   -91,  -2,   -7,   -32,
    -10,  -77,  74,   34,   -122, -39,  103,  -12,  -81,  29,   110,  -10,  5,    17,   -38,  -11,  -55,  -112, 94,   117,  22,   30,   61,   119,  37,   120,
    -126, 71,   -22,  -17,  103,  -85,  -33,  122,  5,    -99,  -3,   -115, 22,   101,  111,  5,    37,   19,   -123, -27,  46,   -13,  8,    -95,  67,   -37,
    -49,  75,   -38,  108,  94,   -47,  -62,  -75,  -88,  -12,  97,   -109, -63,  -67,  100,  -77,  -122, 94,   19,   -72,  -14,  80,   -2,   111,  -90,  89,
    -6,   86,   82,   -95,  -48,  -84,  -54,  66,   -28,  -121, -11,  -61,  19,   70,   -17,  100,  -15,  -30,  80,   -22,  100,  89,   -26,  -36,  5,    -66,
    -117, 102,  88,   -106, 35,   116,  29,   57,   11,   44,   -64,  -113, 34,   93,   6,    12,   110,  -99,  116,  9,    -101, 106,  98,   -83,  -14,  83,
    -44,  53,   38,   108,  -92,  56,   87,   120,  -112, -19,  -40,  85,   -126, -35,  92,   102,  1,    125,  40,   -68,  -96,  -96,  -93,  27,   122,  -113,
    4,    -112, -100, -28,  82,   -111, 55,   -124, -53,  -23,  -45,  -80,  -50,  40,   -29,  11,   49,   55,   5,    -26,  -11,  -88,  -78,  -41,  38,   7,
    -47,  84,   48,   3,    76,   -110, 51,   52,   116,  -48,  14,   -83,  114,  64,   -14,  85,   -1,   17,   103,  22,   39,   -117, 79,   -28,  -7,   11,
    51,   -56,  -32,  75,   118,  51,   -9,   107,  0,    -95,  30,   29,   22,   90,   -63,  -16,  -107, 96,   50,   -5,   -43,  -122, 100,  -11,  -98,  -98,
    29,   33,   -44,  68,   71,   78,   -47,  77,   -21,  -51,  21,   116,  -10,  -53,  -79,  117,  -89,  53,   37,   104,  -60,  106,  -17,  -66,  -42,  26,
    -2,   -28,  -123, 57,   -38,  -109, 124,  7,    -14,  -17,  -113, -46,  55,   -22,  123,  126,  -94,  -55,  117,  93,   57,   51,   85,   60,   -93,  80,
    85,   -7,   -17,  -106, 53,   84,   -14,  -80,  -36,  -119, -94,  -124, -63,  -9,   -119, 98,   -50,  89,   -113, -45,  82,   29,   -11,  87,   -91,  -88,
    -119, -10,  -51,  -86,  -98,  -82,  72,   43,   69,   97,   -118, 5,    50,   -88,  76,   108,  -74,  20,   -105, -102, 119,  -117, 35,   108,  3,    114,
    -46,  -33,  32,   64,   -9,   -128, -78,  27,   49,   82,   -121, 96,   124,  67,   -95,  -26,  -85,  -6,   -77,  26,   26,   62,   -47,  -22,  72,   -80,
    4,    98,   52,   -125, -22,  32,   57,   -18,  42,   112,  -101, 84,   97,   100,  -102, 2,    88,   -34,  -14,  -120, -22,  9,    59,   -40,  -126, 30,
    113,  -49,  35,   -54,  0,    71,   49,   -31,  -25,  37,   88,   23,   22,   82,   -1,   17,   -77,  -19,  48,   -72,  -13,  -99,  -40,  -44,  33,   83,
    2,    -123, 30,   -111, -52,  5,    -45,  102,  -57,  -5,   -41,  111,  -104, 110,  -10,  7,    85,   -60,  -113, 4,    -84,  40,   -50,  -36,  113,  -107,
    12,   17,   111,  -105, -10,  46,   -101, -52,  -69,  68};

static int32_t DeeployNetwork_offset_tensor[1024] = {
    -72.0,  32.0,   113.0,  -47.0,  -56.0,  -55.0,  100.0,  31.0,   -34.0,  -55.0,  -20.0,  48.0,   99.0,   32.0,   -114.0, -67.0,  -41.0,  32.0,   -72.0,
    28.0,   43.0,   -31.0,  50.0,   -82.0,  -20.0,  -56.0,  37.0,   -8.0,   -126.0, -111.0, 15.0,   67.0,   -43.0,  112.0,  114.0,  -35.0,  -27.0,  -8.0,
    22.0,   82.0,   -121.0, -73.0,  49.0,   -102.0, 120.0,  4.0,    -19.0,  -39.0,  -102.0, 60.0,   115.0,  -17.0,  -45.0,  -50.0,  18.0,   -7.0,   -32.0,
    -37.0,  -92.0,  -26.0,  20.0,   -1.0,   108.0,  3.0,    18.0,   77.0,   -77.0,  -79.0,  -97.0,  97.0,   18.0,   -116.0, -17.0,  91.0,   103.0,  -30.0,
    -57.0,  -34.0,  -83.0,  80.0,   -63.0,  -40.0,  -112.0, -83.0,  -1.0,   91.0,   37.0,   50.0,   24.0,   48.0,   76.0,   -124.0, 112.0,  51.0,   46.0,
    -88.0,  14.0,   113.0,  10.0,   27.0,   -50.0,  -26.0,  13.0,   -124.0, 86.0,   -113.0, -99.0,  78.0,   42.0,   25.0,   75.0,   95.0,   91.0,   -106.0,
    -18.0,  -85.0,  84.0,   55.0,   125.0,  2.0,    25.0,   -3.0,   -8.0,   0.0,    -33.0,  56.0,   -18.0,  70.0,   82.0,   -42.0,  62.0,   -82.0,  123.0,
    -39.0,  77.0,   12.0,   -66.0,  -91.0,  83.0,   31.0,   -9.0,   28.0,   -69.0,  120.0,  -45.0,  -88.0,  -24.0,  -7.0,   -39.0,  111.0,  -99.0,  -100.0,
    -76.0,  -86.0,  10.0,   84.0,   -111.0, 0.0,    39.0,   -90.0,  98.0,   65.0,   -80.0,  86.0,   -83.0,  -6.0,   49.0,   -106.0, 103.0,  126.0,  -36.0,
    -63.0,  -43.0,  -62.0,  -96.0,  50.0,   -58.0,  -26.0,  35.0,   117.0,  100.0,  23.0,   115.0,  -6.0,   -46.0,  -26.0,  -39.0,  110.0,  -49.0,  -15.0,
    27.0,   -43.0,  -127.0, -119.0, -13.0,  -47.0,  11.0,   -64.0,  119.0,  -61.0,  121.0,  61.0,   -74.0,  -92.0,  84.0,   -2.0,   -109.0, 111.0,  69.0,
    84.0,   -3.0,   62.0,   68.0,   41.0,   80.0,   16.0,   57.0,   5.0,    -32.0,  -106.0, 3.0,    8.0,    -66.0,  -36.0,  41.0,   -54.0,  -26.0,  -36.0,
    -33.0,  -20.0,  11.0,   34.0,   -37.0,  -79.0,  -57.0,  39.0,   -26.0,  -9.0,   -70.0,  -123.0, -55.0,  -40.0,  12.0,   104.0,  97.0,   -69.0,  78.0,
    109.0,  -113.0, -124.0, 22.0,   52.0,   -62.0,  -57.0,  -19.0,  -56.0,  -123.0, -89.0,  -49.0,  -49.0,  -87.0,  89.0,   22.0,   89.0,   89.0,   19.0,
    -66.0,  -99.0,  -119.0, 11.0,   -3.0,   -29.0,  -102.0, -25.0,  82.0,   -56.0,  1.0,    -29.0,  -14.0,  113.0,  46.0,   -73.0,  -118.0, 74.0,   -104.0,
    -33.0,  -85.0,  91.0,   13.0,   78.0,   -73.0,  -123.0, 23.0,   -86.0,  -25.0,  -79.0,  -27.0,  -26.0,  -53.0,  -11.0,  56.0,   43.0,   24.0,   -47.0,
    -107.0, 94.0,   18.0,   -65.0,  -39.0,  -99.0,  -91.0,  49.0,   40.0,   113.0,  -22.0,  95.0,   -25.0,  -117.0, -72.0,  117.0,  63.0,   14.0,   -96.0,
    65.0,   -7.0,   -11.0,  -63.0,  -92.0,  122.0,  -103.0, 76.0,   -56.0,  -78.0,  -116.0, 24.0,   19.0,   67.0,   10.0,   -121.0, 52.0,   -36.0,  -114.0,
    72.0,   -82.0,  99.0,   -93.0,  81.0,   23.0,   -67.0,  -11.0,  53.0,   -116.0, 45.0,   -121.0, 122.0,  47.0,   -18.0,  39.0,   -34.0,  19.0,   106.0,
    -19.0,  50.0,   -63.0,  43.0,   -46.0,  78.0,   123.0,  -39.0,  -27.0,  -8.0,   56.0,   -56.0,  50.0,   -94.0,  90.0,   -117.0, -20.0,  11.0,   -24.0,
    36.0,   -2.0,   49.0,   76.0,   81.0,   95.0,   74.0,   -80.0,  -54.0,  11.0,   -35.0,  -94.0,  74.0,   76.0,   113.0,  -89.0,  -16.0,  -118.0, -126.0,
    -7.0,   111.0,  -53.0,  46.0,   -77.0,  116.0,  41.0,   122.0,  -37.0,  3.0,    -112.0, -65.0,  -64.0,  -115.0, -90.0,  -124.0, -125.0, 19.0,   -47.0,
    16.0,   -2.0,   -57.0,  24.0,   -117.0, 22.0,   -68.0,  91.0,   2.0,    8.0,    -56.0,  89.0,   -90.0,  -76.0,  97.0,   -40.0,  -65.0,  21.0,   -124.0,
    31.0,   115.0,  75.0,   37.0,   -121.0, 22.0,   -4.0,   -86.0,  59.0,   95.0,   -74.0,  73.0,   -13.0,  51.0,   -46.0,  -105.0, -45.0,  -103.0, -71.0,
    69.0,   -39.0,  -83.0,  -17.0,  -40.0,  51.0,   -69.0,  44.0,   26.0,   -8.0,   -11.0,  43.0,   -7.0,   32.0,   -98.0,  14.0,   -57.0,  -32.0,  61.0,
    -127.0, 118.0,  47.0,   -4.0,   -19.0,  -10.0,  43.0,   90.0,   -113.0, -60.0,  74.0,   -43.0,  39.0,   -22.0,  107.0,  -34.0,  86.0,   -128.0, -47.0,
    58.0,   -116.0, 91.0,   -85.0,  54.0,   -3.0,   47.0,   -106.0, -97.0,  -12.0,  -94.0,  98.0,   -86.0,  -60.0,  -6.0,   -66.0,  -92.0,  -45.0,  44.0,
    81.0,   112.0,  -17.0,  70.0,   -122.0, 48.0,   9.0,    -100.0, -51.0,  56.0,   9.0,    85.0,   -62.0,  -119.0, -18.0,  120.0,  36.0,   -100.0, 116.0,
    -64.0,  74.0,   -4.0,   13.0,   107.0,  -126.0, 37.0,   71.0,   -41.0,  94.0,   81.0,   -125.0, -17.0,  98.0,   -48.0,  115.0,  -46.0,  -78.0,  61.0,
    106.0,  82.0,   -100.0, -93.0,  -75.0,  -102.0, -15.0,  -25.0,  -86.0,  -82.0,  69.0,   -100.0, -1.0,   43.0,   -45.0,  -66.0,  -54.0,  74.0,   9.0,
    91.0,   99.0,   -53.0,  -32.0,  38.0,   -97.0,  115.0,  22.0,   -2.0,   -37.0,  -71.0,  -107.0, 97.0,   -67.0,  108.0,  -87.0,  -13.0,  -21.0,  -29.0,
    41.0,   122.0,  23.0,   125.0,  104.0,  -76.0,  -105.0, -6.0,   -56.0,  -87.0,  109.0,  -68.0,  -81.0,  15.0,   -92.0,  65.0,   -34.0,  101.0,  -86.0,
    126.0,  -128.0, 83.0,   6.0,    93.0,   -61.0,  -97.0,  -99.0,  -112.0, 67.0,   -102.0, 125.0,  -122.0, 105.0,  101.0,  -117.0, -47.0,  -73.0,  -3.0,
    -85.0,  -61.0,  -108.0, 91.0,   -9.0,   92.0,   -114.0, -114.0, -48.0,  99.0,   63.0,   -11.0,  102.0,  -85.0,  27.0,   -26.0,  87.0,   20.0,   123.0,
    -105.0, -94.0,  -96.0,  -63.0,  -106.0, -107.0, 57.0,   -60.0,  -83.0,  104.0,  39.0,   -104.0, -42.0,  -111.0, 52.0,   90.0,   8.0,    -123.0, 97.0,
    -42.0,  122.0,  -46.0,  -118.0, -66.0,  -99.0,  11.0,   30.0,   -57.0,  -70.0,  94.0,   -86.0,  85.0,   -57.0,  25.0,   108.0,  -74.0,  -10.0,  -45.0,
    -103.0, 45.0,   -7.0,   106.0,  3.0,    -12.0,  84.0,   113.0,  -45.0,  121.0,  -91.0,  33.0,   60.0,   -20.0,  89.0,   -121.0, 60.0,   37.0,   -44.0,
    122.0,  -30.0,  -112.0, 45.0,   104.0,  39.0,   -95.0,  -31.0,  -63.0,  73.0,   -41.0,  -60.0,  -104.0, -122.0, -31.0,  -14.0,  -68.0,  88.0,   -67.0,
    116.0,  59.0,   119.0,  -7.0,   74.0,   -99.0,  111.0,  120.0,  -100.0, 24.0,   3.0,    82.0,   21.0,   -122.0, -44.0,  -107.0, -9.0,   -101.0, 26.0,
    89.0,   64.0,   -18.0,  3.0,    -75.0,  -91.0,  -121.0, -8.0,   -48.0,  75.0,   11.0,   108.0,  2.0,    -74.0,  -123.0, -111.0, 122.0,  -3.0,   -50.0,
    5.0,    -50.0,  -47.0,  105.0,  -18.0,  -20.0,  -98.0,  -30.0,  -125.0, 0.0,    -35.0,  -84.0,  44.0,   108.0,  -1.0,   -13.0,  61.0,   126.0,  -128.0,
    -111.0, 29.0,   -37.0,  34.0,   114.0,  115.0,  100.0,  59.0,   -107.0, -65.0,  94.0,   120.0,  -27.0,  17.0,   -90.0,  79.0,   1.0,    15.0,   105.0,
    86.0,   109.0,  -111.0, 108.0,  53.0,   22.0,   77.0,   -14.0,  47.0,   66.0,   -117.0, 9.0,    114.0,  -88.0,  54.0,   51.0,   -81.0,  -124.0, 10.0,
    113.0,  126.0,  6.0,    -10.0,  44.0,   -56.0,  8.0,    72.0,   108.0,  101.0,  -110.0, -126.0, 69.0,   93.0,   121.0,  101.0,  -28.0,  -22.0,  29.0,
    27.0,   17.0,   58.0,   11.0,   -79.0,  -56.0,  -40.0,  -125.0, -16.0,  -117.0, -71.0,  50.0,   15.0,   57.0,   3.0,    -10.0,  100.0,  60.0,   39.0,
    126.0,  85.0,   29.0,   53.0,   -61.0,  -72.0,  75.0,   -9.0,   -49.0,  -83.0,  -38.0,  -49.0,  97.0,   -117.0, 119.0,  5.0,    30.0,   58.0,   -60.0,
    -69.0,  -96.0,  -91.0,  43.0,   -3.0,   -68.0,  -83.0,  -99.0,  122.0,  27.0,   103.0,  -101.0, -99.0,  -2.0,   -56.0,  -53.0,  -31.0,  78.0,   -90.0,
    -117.0, 74.0,   118.0,  -39.0,  -96.0,  50.0,   -88.0,  70.0,   -98.0,  22.0,   -5.0,   -53.0,  -93.0,  84.0,   68.0,   -65.0,  -124.0, 49.0,   -12.0,
    29.0,   63.0,   -67.0,  -37.0,  -17.0,  83.0,   -24.0,  -38.0,  -102.0, -55.0,  50.0,   95.0,   -34.0,  -111.0, 71.0,   -72.0,  115.0,  -93.0,  73.0,
    92.0,   -81.0,  -80.0,  -9.0,   71.0,   -11.0,  -61.0,  -5.0,   71.0,   -66.0,  72.0,   -91.0,  -123.0, -31.0,  -73.0,  92.0,   46.0,   -90.0,  -79.0,
    -120.0, -6.0,   65.0,   -62.0,  -66.0,  -112.0, 61.0,   66.0,   -117.0, 31.0,   10.0,   -108.0, -28.0,  -114.0, -8.0,   -76.0,  71.0,   29.0,   5.0,
    -114.0, -14.0,  110.0,  79.0,   -77.0,  -96.0,  23.0,   121.0,  -37.0,  -122.0, 73.0,   -8.0,   -63.0,  -53.0,  -41.0,  124.0,  70.0,   -88.0,  60.0,
    -92.0,  12.0,   35.0,   -66.0,  69.0,   -66.0,  58.0,   -52.0,  -110.0, 0.0,    -127.0, 84.0,   77.0,   -38.0,  -16.0,  -21.0,  21.0,   -50.0,  -115.0,
    104.0,  -61.0,  -23.0,  17.0,   -123.0, -102.0, 63.0,   27.0,   107.0,  109.0,  41.0,   -115.0, -83.0,  48.0,   -72.0,  -70.0,  -111.0};

static uint8_t DeeployNetwork_TILING_CODEGEN_L1_GEMM1_numTiles[2] = {0, 1};

void *DeeployNetwork_inputs[1];
void *DeeployNetwork_outputs[1];
typedef struct {
  int8_t *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_ref;
  int8_t *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_ref;
  int32_t *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_ref;
  int32_t *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_data_out_ref;
} _GEMM1_tiling_closure_args_t;

static void _GEMM1_tiling_closure(void *_GEMM1_tiling_closure_args) {
  // CLOSURE ARG CAST
  _GEMM1_tiling_closure_args_t *args = (_GEMM1_tiling_closure_args_t *)_GEMM1_tiling_closure_args;

  int8_t *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_ref = args->DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_ref;
  int8_t *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_ref = args->DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_ref;
  int32_t *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_ref = args->DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_ref;
  int32_t *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_data_out_ref = args->DeeployNetwork_TILING_CODEGEN_L1_GEMM1_data_out_ref;

  // CLOSURE FUNCTION CALL
  if (snrt_is_compute_core()) {

    Gemm_s8_transB_row_parallel(DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_ref, DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_ref,
                                DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_ref, DeeployNetwork_TILING_CODEGEN_L1_GEMM1_data_out_ref, 32, 32, 32, 1, 1);
  }

  // CLOSURE ARG WRITEBACK
}

typedef struct {
  uint8_t *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_tileIdxPtr;
} _GEMM1_closure_args_t;

static void _GEMM1_closure(void *_GEMM1_closure_args) {
  // CLOSURE ARG CAST
  _GEMM1_closure_args_t *args = (_GEMM1_closure_args_t *)_GEMM1_closure_args;

  uint8_t *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_tileIdxPtr = args->DeeployNetwork_TILING_CODEGEN_L1_GEMM1_tileIdxPtr;

  // CLOSURE FUNCTION CALL
  int8_t *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_ref = (int8_t *)((char *)DeeployNetwork_MEMORYARENA_L1 + 16384);
  int8_t *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_ref = (int8_t *)((char *)DeeployNetwork_MEMORYARENA_L1 + 18432);
  int32_t *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_ref = (int32_t *)((char *)DeeployNetwork_MEMORYARENA_L1 + 8192);
  int32_t *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_data_out_ref = (int32_t *)((char *)DeeployNetwork_MEMORYARENA_L1 + 0);
  void *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_input_0_ref = (void *)((char *)DeeployNetwork_input_0 + 0);
  void *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_input_0_buffer_0 = (void *)((char *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_ref + 0);
  void *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_input_0_buffer_1 = (void *)((char *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_ref + 1024);
  void *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_next = (void *)((char *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_input_0_buffer_1 + 0);
  void *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_weight_tensor_ref = (void *)((char *)DeeployNetwork_weight_tensor + 0);
  void *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_weight_tensor_buffer_0 = (void *)((char *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_ref + 0);
  void *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_weight_tensor_buffer_1 = (void *)((char *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_ref + 1024);
  void *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_next = (void *)((char *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_weight_tensor_buffer_1 + 0);
  void *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_offset_tensor_ref = (void *)((char *)DeeployNetwork_offset_tensor + 0);
  void *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_offset_tensor_buffer_0 = (void *)((char *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_ref + 0);
  void *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_offset_tensor_buffer_1 = (void *)((char *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_ref + 4096);
  void *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_next = (void *)((char *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_offset_tensor_buffer_1 + 0);
  void *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_output_0_ref = (void *)((char *)DeeployNetwork_output_0 + 0);
  void *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_output_0_buffer_0 = (void *)((char *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_data_out_ref + 0);
  void *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_output_0_buffer_1 = (void *)((char *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_data_out_ref + 4096);

  // Initialize DMA future
  snrt_dma_txid_t data_out_LocalToExternal = (snrt_dma_txid_t)-1;
  snrt_dma_txid_t C_ExternalToLocal = (snrt_dma_txid_t)-1;
  snrt_dma_txid_t B_ExternalToLocal = (snrt_dma_txid_t)-1;
  snrt_dma_txid_t A_ExternalToLocal = (snrt_dma_txid_t)-1;

  if (snrt_is_dm_core()) {
    A_ExternalToLocal =
        snrt_dma_start_2d(DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_ref, DeeployNetwork_TILING_CODEGEN_L1_GEMM1_input_0_ref, 1024, 1024, 1024, 1);
    // WIESEP: Hack as otherwise the last commited DMA transaction ID can never be resolved.
    snrt_dma_start_2d(DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_ref, DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_ref, 1, 0, 0, 0);
  }

  if (snrt_is_dm_core()) {
    B_ExternalToLocal =
        snrt_dma_start_2d(DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_ref, DeeployNetwork_TILING_CODEGEN_L1_GEMM1_weight_tensor_ref, 1024, 1024, 1024, 1);
    // WIESEP: Hack as otherwise the last commited DMA transaction ID can never be resolved.
    snrt_dma_start_2d(DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_ref, DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_ref, 1, 0, 0, 0);
  }

  if (snrt_is_dm_core()) {
    C_ExternalToLocal =
        snrt_dma_start_2d(DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_ref, DeeployNetwork_TILING_CODEGEN_L1_GEMM1_offset_tensor_ref, 4096, 4096, 4096, 1);
    // WIESEP: Hack as otherwise the last commited DMA transaction ID can never be resolved.
    snrt_dma_start_2d(DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_ref, DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_ref, 1, 0, 0, 0);
  }

  // TILING LOOP
  for (int TILING_I = DeeployNetwork_TILING_CODEGEN_L1_GEMM1_numTiles[*DeeployNetwork_TILING_CODEGEN_L1_GEMM1_tileIdxPtr];
       TILING_I < DeeployNetwork_TILING_CODEGEN_L1_GEMM1_numTiles[(*DeeployNetwork_TILING_CODEGEN_L1_GEMM1_tileIdxPtr) + 1]; TILING_I++) {

    switch ((TILING_I) % 2) {
    case 0:
      DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_ref = (int8_t *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_input_0_buffer_0;
      DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_ref = (int8_t *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_weight_tensor_buffer_0;
      DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_ref = (int32_t *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_offset_tensor_buffer_0;
      DeeployNetwork_TILING_CODEGEN_L1_GEMM1_data_out_ref = (int32_t *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_output_0_buffer_0;
      break;
    case 1:
      DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_ref = (int8_t *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_input_0_buffer_1;
      DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_ref = (int8_t *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_weight_tensor_buffer_1;
      DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_ref = (int32_t *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_offset_tensor_buffer_1;
      DeeployNetwork_TILING_CODEGEN_L1_GEMM1_data_out_ref = (int32_t *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_output_0_buffer_1;
      break;
    }

    // Wait for current input tile
    if ((A_ExternalToLocal != ((snrt_dma_txid_t)-1)) && snrt_is_dm_core())
      snrt_dma_wait(A_ExternalToLocal);

    // DOUBLE BUFFERING CHECK TILE LOAD
    if ((TILING_I + 1) < DeeployNetwork_TILING_CODEGEN_L1_GEMM1_numTiles[*DeeployNetwork_TILING_CODEGEN_L1_GEMM1_tileIdxPtr + 1]) {

      switch ((TILING_I + 1) % 2) {
      case 0:
        DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_next = (void *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_input_0_buffer_0;
        break;
      case 1:
        DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_next = (void *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_input_0_buffer_1;
        break;
      }

      // Transfer next input tile

      if (snrt_is_dm_core()) {
        A_ExternalToLocal =
            snrt_dma_start_2d(DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_next, DeeployNetwork_TILING_CODEGEN_L1_GEMM1_input_0_ref, 1024, 1024, 1024, 1);
        // WIESEP: Hack as otherwise the last commited DMA transaction ID can never be resolved.
        snrt_dma_start_2d(DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_next, DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_next, 1, 0, 0, 0);
      }
    }

    // Wait for current input tile
    if ((B_ExternalToLocal != ((snrt_dma_txid_t)-1)) && snrt_is_dm_core())
      snrt_dma_wait(B_ExternalToLocal);

    // DOUBLE BUFFERING CHECK TILE LOAD
    if ((TILING_I + 1) < DeeployNetwork_TILING_CODEGEN_L1_GEMM1_numTiles[*DeeployNetwork_TILING_CODEGEN_L1_GEMM1_tileIdxPtr + 1]) {

      switch ((TILING_I + 1) % 2) {
      case 0:
        DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_next = (void *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_weight_tensor_buffer_0;
        break;
      case 1:
        DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_next = (void *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_weight_tensor_buffer_1;
        break;
      }

      // Transfer next input tile

      if (snrt_is_dm_core()) {
        B_ExternalToLocal =
            snrt_dma_start_2d(DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_next, DeeployNetwork_TILING_CODEGEN_L1_GEMM1_weight_tensor_ref, 1024, 1024, 1024, 1);
        // WIESEP: Hack as otherwise the last commited DMA transaction ID can never be resolved.
        snrt_dma_start_2d(DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_next, DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_next, 1, 0, 0, 0);
      }
    }

    // Wait for current input tile
    if ((C_ExternalToLocal != ((snrt_dma_txid_t)-1)) && snrt_is_dm_core())
      snrt_dma_wait(C_ExternalToLocal);

    // DOUBLE BUFFERING CHECK TILE LOAD
    if ((TILING_I + 1) < DeeployNetwork_TILING_CODEGEN_L1_GEMM1_numTiles[*DeeployNetwork_TILING_CODEGEN_L1_GEMM1_tileIdxPtr + 1]) {

      switch ((TILING_I + 1) % 2) {
      case 0:
        DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_next = (void *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_offset_tensor_buffer_0;
        break;
      case 1:
        DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_next = (void *)DeeployNetwork_TILING_CODEGEN_L1_GEMM1_offset_tensor_buffer_1;
        break;
      }

      // Transfer next input tile

      if (snrt_is_dm_core()) {
        C_ExternalToLocal =
            snrt_dma_start_2d(DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_next, DeeployNetwork_TILING_CODEGEN_L1_GEMM1_offset_tensor_ref, 4096, 4096, 4096, 1);
        // WIESEP: Hack as otherwise the last commited DMA transaction ID can never be resolved.
        snrt_dma_start_2d(DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_next, DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_next, 1, 0, 0, 0);
      }
    }

    snrt_cluster_hw_barrier();

    _GEMM1_tiling_closure_args_t DeeployNetwork__GEMM1_tiling_closure_args =
        (_GEMM1_tiling_closure_args_t){.DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_ref = DeeployNetwork_TILING_CODEGEN_L1_GEMM1_A_ref,
                                       .DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_ref = DeeployNetwork_TILING_CODEGEN_L1_GEMM1_B_ref,
                                       .DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_ref = DeeployNetwork_TILING_CODEGEN_L1_GEMM1_C_ref,
                                       .DeeployNetwork_TILING_CODEGEN_L1_GEMM1_data_out_ref = DeeployNetwork_TILING_CODEGEN_L1_GEMM1_data_out_ref};

    // _GEMM1_tiling_closure CLOSURE CALL
    _GEMM1_tiling_closure(&DeeployNetwork__GEMM1_tiling_closure_args);

    snrt_cluster_hw_barrier();

    // Wait for previous output tile
    if ((data_out_LocalToExternal != ((snrt_dma_txid_t)-1)) && snrt_is_dm_core())
      snrt_dma_wait(data_out_LocalToExternal);

    // Transfer current output tile

    if (snrt_is_dm_core()) {
      data_out_LocalToExternal =
          snrt_dma_start_2d(DeeployNetwork_TILING_CODEGEN_L1_GEMM1_output_0_ref, DeeployNetwork_TILING_CODEGEN_L1_GEMM1_data_out_ref, 4096, 4096, 4096, 1);
      // WIESEP: Hack as otherwise the last commited DMA transaction ID can never be resolved.
      snrt_dma_start_2d(DeeployNetwork_TILING_CODEGEN_L1_GEMM1_output_0_ref, DeeployNetwork_TILING_CODEGEN_L1_GEMM1_output_0_ref, 1, 0, 0, 0);
    }

    // CLOSE TILING LOOP
  }
  *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_tileIdxPtr += 1;

  // Wait for final output tile
  if ((data_out_LocalToExternal != ((snrt_dma_txid_t)-1)) && snrt_is_dm_core())
    snrt_dma_wait(data_out_LocalToExternal);

  // Deinitialize DMA future

  // CLOSURE ARG WRITEBACK
}

void RunNetwork(__attribute__((unused)) uint32_t core_id, __attribute__((unused)) uint32_t numThreads) {
  {

    uint8_t bu_DeeployNetwork_TILING_CODEGEN_L1_GEMM1_tileIdxPtr = 0;
    uint8_t *DeeployNetwork_TILING_CODEGEN_L1_GEMM1_tileIdxPtr = &bu_DeeployNetwork_TILING_CODEGEN_L1_GEMM1_tileIdxPtr;
    _GEMM1_closure_args_t DeeployNetwork__GEMM1_closure_args =
        (_GEMM1_closure_args_t){.DeeployNetwork_TILING_CODEGEN_L1_GEMM1_tileIdxPtr = DeeployNetwork_TILING_CODEGEN_L1_GEMM1_tileIdxPtr};

    // _GEMM1_closure CLOSURE CALL
    _GEMM1_closure(&DeeployNetwork__GEMM1_closure_args);
  }
}

void InitNetwork(__attribute__((unused)) uint32_t core_id, __attribute__((unused)) uint32_t numThreads) {

  DeeployNetwork_MEMORYARENA_L1 = (int8_t *)snrt_l1_alloc(sizeof(int8_t) * 20480);

  DeeployNetwork_MEMORYARENA_L2 = (int8_t *)snrt_l3_alloc(sizeof(int8_t) * 5120);
  DeeployNetwork_input_0 = (int8_t *)((char *)DeeployNetwork_MEMORYARENA_L2 + 4096);
  DeeployNetwork_output_0 = (int32_t *)((char *)DeeployNetwork_MEMORYARENA_L2 + 0);
  DeeployNetwork_inputs[0] = (void *)DeeployNetwork_input_0;
  DeeployNetwork_outputs[0] = (void *)DeeployNetwork_output_0;
}
