const int circle_8_length = 798;
const char circle_8[circle_8_length] = {
	-119, 80, 78, 71, 13, 10, 26, 10, 0, 0, 0, 13, 73, 72, 68, 82, 
	0, 0, 0, 24, 0, 0, 0, 24, 8, 6, 0, 0, 0, -32, 119, 61, 
	-8, 0, 0, 0, 6, 98, 75, 71, 68, 0, -1, 0, -1, 0, -1, -96, 
	-67, -89, -109, 0, 0, 0, 9, 112, 72, 89, 115, 0, 0, 13, -41, 0, 
	0, 13, -41, 1, 66, 40, -101, 120, 0, 0, 0, 7, 116, 73, 77, 69, 
	7, -29, 10, 18, 1, 4, 53, -33, -71, 47, 41, 0, 0, 2, -85, 73, 
	68, 65, 84, 72, -57, -83, -42, -49, 111, 84, 85, 20, 7, -16, -49, 123, 
	37, 113, -26, -35, -23, -69, -45, 66, -116, -76, 5, -38, 116, -38, -8, 35, 
	33, 97, -81, 44, 68, 23, -2, -86, -82, 116, 105, -30, 86, -27, 95, 16, 
	-73, 34, -18, -63, 31, 113, 43, -72, 82, 113, 39, -126, 70, -127, 29, 36, 
	10, 38, -2, 90, 96, -93, -47, -76, 9, 83, -61, 52, 105, 103, -98, -117, 
	-34, 22, -93, -99, 1, -38, -98, -28, -28, 38, -25, -34, -9, -3, -98, 119, 
	-18, -67, -33, 115, 51, 3, 108, -92, 110, 127, 119, -56, 92, 86, 121, 6, 
	-109, -40, -105, -90, 110, -32, -41, 42, 115, 118, 87, -41, 39, -117, 29, 55, 
	-6, 97, 100, -101, 5, 71, -21, 38, -70, -71, 55, -15, 50, -122, 42, -66, 
	-49, -71, -46, -29, 79, -56, -71, 31, -121, 42, 30, 70, 23, 31, -18, -22, 
	121, 99, -95, 99, -34, -99, -84, 89, 120, 33, 22, -106, 98, -48, -119, -63, 
	-15, 24, 77, -9, 91, 27, -93, -23, 24, -68, -99, -42, -74, -101, -123, -25, 
	7, -126, -105, 13, 71, 99, -48, 109, 22, -66, 25, -87, -37, -97, -62, 79, 
	-30, 93, -4, -128, -65, -109, 95, -57, 41, 60, 1, -51, -102, 3, -79, -31, 
	98, 12, -70, 101, -61, -47, -2, -103, 7, -35, 24, -100, -58, 125, -104, -59, 
	5, 84, 119, -16, -13, -104, 65, 45, 22, -50, -92, 4, -25, -2, 87, -13, 
	88, 88, -118, 13, -33, 38, -16, -61, 88, -68, 11, -16, 117, 95, -60, 99, 
	-88, -59, -32, 82, 12, -38, 123, 10, 99, -73, 107, 25, -68, 31, -125, 78, 
	42, -53, -20, 61, -126, -81, -5, 2, 90, -51, -102, -55, 24, 44, -57, -122, 
	83, 27, 71, 49, 6, -85, 49, 120, 43, -15, -99, -37, 2, -8, -70, 95, 
	-128, 50, 56, 17, -125, -43, -47, -70, 9, 101, -61, -21, 49, -88, -54, -46, 
	76, -38, -48, 106, -101, 126, -92, 44, -75, 98, 80, -107, 13, -81, -26, 89, 
	-27, 41, 124, -41, 110, -5, 17, 47, -39, -66, -67, -40, 110, -5, 9, -41, 
	-77, -54, -45, 57, 90, 25, 87, -45, -28, -93, 59, 64, 112, 56, -115, 87, 
	49, -99, -85, -20, -83, -8, 61, 5, -57, 119, -128, 96, 34, -115, -13, 42, 
	-29, -71, 76, -43, 79, 50, -74, 104, 61, 107, -101, -111, -55, -12, 114, 107, 
	-39, -113, 109, -80, 110, -33, -26, -109, -56, -115, 99, 62, -57, -49, 21, 7, 
	-45, -28, -41, 59, 64, -16, 85, 26, 15, 102, -4, -110, 87, -103, -49, 51, 
	30, 25, 30, 54, -117, -113, 118, -128, -32, 116, 89, 106, -31, -95, 42, 115, 
	-42, 104, -35, -66, 116, -47, -114, -89, 5, 95, 108, -29, 14, -100, 79, 23, 
	-19, -99, 24, -84, -20, -82, -89, 67, 19, -125, -9, 98, -48, 105, -42, 28, 
	-64, 20, -2, -38, -86, 84, -60, -102, -87, 36, 21, 39, 55, -2, 105, 119, 
	-35, 120, 12, -38, -79, -31, 34, 106, 73, -72, 22, -18, 17, 124, 93, -20, 
	46, -57, -32, 102, 81, -40, -5, 95, -71, -98, -117, 65, 55, 22, -50, 36, 
	-110, 25, 124, 121, 23, -32, -25, -48, 74, 114, -3, 113, 12, -70, -61, -123, 
	103, 7, 54, -100, 24, 92, 106, -42, 76, -90, -16, 17, -100, -60, 53, 44, 
	37, -65, -106, 98, -113, 67, 42, -53, -27, -44, 112, 94, 27, 120, 4, 70, 
	10, -49, -59, -96, 29, -125, -27, 50, 56, -111, 68, 112, -13, -124, 74, 51, 
	105, 67, -105, 99, 112, 115, -77, -52, 55, -67, -63, 123, 10, 99, 43, -71, 
	99, 42, -81, 96, 40, -75, -56, 43, 21, 127, -92, -113, 30, -64, 33, 60, 
	-120, 85, -103, 15, 86, 122, -114, -35, -70, -75, 33, 57, -125, 9, -2, -35, 
	-23, 86, 111, 63, 91, -90, 84, -23, -39, -110, -83, 61, 91, 100, 62, 27, 
	-22, -6, 116, -79, -29, -73, 126, 24, -1, 0, 105, -114, 22, 59, -115, -30, 
	1, 22, 0, 0, 0, 0, 73, 69, 78, 68, -82, 66, 96, -126
};
