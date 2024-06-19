/* Includes *****************************************************************/

#include "stdafx.h"

/* Defines ******************************************************************/

#define STRING_SIZE		256
#define DATA_SIZE		256

/* Structs ******************************************************************/

struct display_instance
{
	BYTE data[DATA_SIZE];
	TCHAR display_id[STRING_SIZE];
	TCHAR instance_id[STRING_SIZE];
	TCHAR name[STRING_SIZE];
	BOOL name_status;
	BOOL active_status;
	BOOL override_status;
	BOOL active_override_status;
	struct display_instance *next;
};

/****************************************************************************/

struct display_instance_list
{
	int count;
	int active_count;
	int override_count;
	int active_override_count;
	struct display_instance *first;
	struct display_instance *last;
};

/****************************************************************************/

struct display
{
	TCHAR name[STRING_SIZE];
	struct display_instance_list *instance_list;
	struct display *next;
};

/****************************************************************************/

struct display_list
{
	int count;
	int active_count;
	int override_count;
	int active_override_count;
	struct display *first;
	struct display *last;
};

/****************************************************************************/

struct resolution
{
	int width;
	int height;
	int vrate;
	int hfront;
	int hsync;
	int hback;
	int vfront;
	int vsync;
	int vback;
	int hpolarity;
	int vpolarity;
};

/* Resolutions **************************************************************/

#define RESOLUTIONS				8
#define STROBED_RESOLUTIONS		3

const struct resolution resolution_list[RESOLUTIONS] =
{
	{1920,  1080,  1200000,  48,  32,  80,  3,  5,  61,  TRUE,  TRUE},
	{1920,  1080,  1100000,  48,  32,  80,  3,  5,  55,  TRUE,  TRUE},
	{1920,  1080,  1000000,  48,  32,  80,  3,  5,  50,  TRUE,  TRUE},
	{1920,  1080,  1440000,  24,  32,  32,  3,  5,  10,  TRUE,  TRUE},
	{1920,  1080,  1205000,  48,  32,  80,  3,  5,  55,  TRUE,  FALSE},
	{1920,  1080,  1105000,  48,  32,  80,  3,  5,  49,  TRUE,  FALSE},
	{1920,  1080,  1005000,  48,  32,  80,  3,  5,  44,  TRUE,  FALSE},
	{1920,  1080,   600000,  88,  44, 148,  4,  5,  36,  TRUE,  TRUE},
/*
	{1920,  1080,  1205000,  48,  32,  80,  3,  5,  61,  TRUE,  TRUE},
	{1920,  1080,  1105000,  48,  32,  80,  3,  5,  55,  TRUE,  TRUE},
	{1920,  1080,  1005000,  48,  32,  80,  3,  5,  50,  TRUE,  TRUE},
	{1920,  1080,  1440000,  24,  32,  32,  3,  5,  10,  TRUE,  TRUE},
	{1920,  1080,  1200000,  48,  32,  80,  3,  5,  56,  TRUE,  FALSE},
	{1920,  1080,  1100000,  48,  32,  80,  3,  5,  50,  TRUE,  FALSE},
	{1920,  1080,  1000000,  48,  32,  80,  3,  5,  45,  TRUE,  FALSE},
	{1920,  1080,   600000,  88,  44, 148,  4,  5,  36,  TRUE,  TRUE},
*/
};

/* Frequencies **************************************************************/

const int frequencies[] = {144, 121, 120, 111, 110, 101, 100, 0};

/* Functions ****************************************************************/

BOOL list_displays(const struct display_list *display_list, HWND window, int item, int selection)
{
	const struct display *display;

	if (!display_list)
		return FALSE;

	for (display = display_list->first; display; display = display->next)
		SendDlgItemMessage(window, item, CB_ADDSTRING, 0, (LPARAM)display->name);

	SendDlgItemMessage(window, item, CB_SETCURSEL, selection, 0);
	return TRUE;
}

/****************************************************************************/

struct display_instance *new_display_instance()
{
	struct display_instance *display_instance;

	display_instance = (struct display_instance *)malloc(sizeof *display_instance);
	memset(display_instance, 0, sizeof *display_instance);
	return display_instance;
}

/****************************************************************************/

struct display_instance_list *new_display_instance_list()
{
	struct display_instance_list *display_instance_list;

	display_instance_list = (struct display_instance_list *)malloc(sizeof *display_instance_list);
	memset(display_instance_list, 0, sizeof *display_instance_list);
	return display_instance_list;
}

/****************************************************************************/

struct display *new_display()
{
	struct display *display;

	display = (struct display *)malloc(sizeof *display);
	memset(display, 0, sizeof *display);
	return display;
}

/****************************************************************************/

struct display_list *new_display_list()
{
	struct display_list *display_list;

	display_list = (struct display_list *)malloc(sizeof *display_list);
	memset(display_list, 0, sizeof *display_list);
	return display_list;
}

/****************************************************************************/

BOOL add_display_instance(struct display_list *display_list, struct display_instance *display_instance)
{
	struct display *display;

	if (!display_list || !display_instance)
		return FALSE;

	for (display = display_list->first; display; display = display->next)
	{
		if (!display->instance_list)
			continue;

		if (!display->instance_list->first)
			continue;

		if (_tcscmp(display_instance->name, display->instance_list->first->name) == 0)
		{
			display->instance_list->last->next = display_instance;
			display->instance_list->last = display_instance;
			break;
		}
	}

	if (!display)
	{
		display = new_display();
		display->instance_list = new_display_instance_list();
		display->instance_list->first = display_instance;
		display->instance_list->last = display_instance;

		if (!display_list->first)
			display_list->first = display;
		else
			display_list->last->next = display;

		display_list->last = display;
	}

	display->instance_list->count++;
	display->instance_list->active_count += display_instance->active_status;
	display->instance_list->override_count += display_instance->override_status;
	display->instance_list->active_override_count += display_instance->active_override_status;
	display_list->count++;
	display_list->active_count += display_instance->active_status;
	display_list->override_count += display_instance->override_status;
	display_list->active_override_count += display_instance->active_override_status;

	if (display->instance_list->active_count == 0)
		_sntprintf(display->name, STRING_SIZE, _T("%s%s"), display->instance_list->first->name, display->instance_list->override_count ? _T("*") : _T(""));
	else
		_sntprintf(display->name, STRING_SIZE, _T("%s (%i active)%s"), display->instance_list->first->name, display->instance_list->active_count, display->instance_list->override_count ? _T("*") : _T(""));
/*
	if (display->instance_list->active_count == 0)
		_sntprintf(display->name, STRING_SIZE, _T("%s%s"), display->instance_list->first->name, display->instance_list->override_count ? _T("*") : _T(""));
	else if (display->instance_list->active_override_count == 0)
		_sntprintf(display->name, STRING_SIZE, _T("%s (%i not installed)%s"), display->instance_list->first->name, display->instance_list->active_count, display->instance_list->override_count ? _T("*") : _T(""));
	else if (display->instance_list->active_count != display->instance_list->active_override_count)
		_sntprintf(display->name, STRING_SIZE, _T("%s (%i of %i installed)%s"), display->instance_list->first->name, display->instance_list->active_override_count, display->instance_list->active_count, display->instance_list->override_count ? _T("*") : _T(""));
	else
		_sntprintf(display->name, STRING_SIZE, _T("%s (%i installed)%s"), display->instance_list->first->name, display->instance_list->active_override_count, display->instance_list->override_count ? _T("*") : _T(""));
*/
	display->name[STRING_SIZE - 1] = 0;
	return TRUE;
}

/****************************************************************************/

BOOL load_active_data(LPBYTE data, LPCTSTR display_id, LPCTSTR instance_id)
{
	int count;
	HKEY key;
	TCHAR subkey[STRING_SIZE];
	DWORD size;
	LONG result;

	if (!data || !display_id || !instance_id)
		return FALSE;

	count = _sntprintf(subkey, STRING_SIZE, _T("SYSTEM\\CurrentControlSet\\Enum\\DISPLAY\\%s\\%s\\Device Parameters"), display_id, instance_id);

	if (count <= 0 || count >= STRING_SIZE)
		return FALSE;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
		return FALSE;

	size = DATA_SIZE;
	result = RegQueryValueEx(key, _T("EDID"), NULL, NULL, data, &size);
	RegCloseKey(key);

	if (result != ERROR_SUCCESS)
		return FALSE;

	if (size < 128)
		return FALSE;

	return TRUE;
}

/****************************************************************************/

BOOL load_active_status(LPCTSTR display_id, LPCTSTR instance_id)
{
	int count;
	TCHAR subkey[STRING_SIZE];
	HDEVINFO device_list;
	SP_DEVINFO_DATA device;
	BOOL status;

	count = _sntprintf(subkey, STRING_SIZE, _T("DISPLAY\\%s\\%s"), display_id, instance_id);

	if (count <= 0 || count >= STRING_SIZE)
		return FALSE;

	device_list = SetupDiGetClassDevs(&GUID_DEVINTERFACE_MONITOR, subkey, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

	if (device_list == INVALID_HANDLE_VALUE)
		return FALSE;

	device.cbSize = sizeof device;
	status = SetupDiEnumDeviceInfo(device_list, 0, &device);
	SetupDiDestroyDeviceInfoList(device_list);
	return status;
}

/****************************************************************************/

BOOL load_override_data(LPBYTE data, LPCTSTR display_id, LPCTSTR instance_id)
{
	int count;
	HKEY key;
	TCHAR subkey[STRING_SIZE];
	DWORD size;
	LONG result;

	if (!data || !display_id || !instance_id)
		return FALSE;

	count = _sntprintf(subkey, STRING_SIZE, _T("SYSTEM\\CurrentControlSet\\Enum\\DISPLAY\\%s\\%s\\Device Parameters\\EDID_OVERRIDE"), display_id, instance_id);

	if (count <= 0 || count >= STRING_SIZE)
		return FALSE;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
		return FALSE;

	size = DATA_SIZE;
	result = RegQueryValueEx(key, _T("0"), NULL, NULL, data, &size);
	RegCloseKey(key);

	if (result != ERROR_SUCCESS)
		return FALSE;

	if (size < 128)
		return FALSE;

	return TRUE;
}

/****************************************************************************/

BOOL read_display_name(LPTSTR name, LPCBYTE data)
{
	int index;
	LPCBYTE src;
	LPTSTR dst;
	int count;

	if (!name || !data)
		return FALSE;

	for (index = 0; index < 4; index++)
	{
		src = &data[54 + index * 18];

		if (memcmp(src, "\x00\x00\x00\xFC\x00", 5) == 0)
		{
			src += 5;

			if (*src >= ' ')
			{
				for (dst = name, count = 0; *src >= ' ' && count < 13; src++, dst++, count++)
					*dst = *src;

				*dst = 0;
				return TRUE;
			}
		}
	}

	return FALSE;
}

/****************************************************************************/

BOOL load_backup_name(LPTSTR name, LPCTSTR display_id, LPCTSTR instance_id)
{
	int count;
	HKEY key;
	TCHAR subkey[STRING_SIZE];
	TCHAR buffer[STRING_SIZE];
	DWORD size;
	LONG result;
	LPTSTR src, dst;

	if (!name || !display_id || !instance_id)
		return FALSE;

	count = _sntprintf(subkey, STRING_SIZE, _T("SYSTEM\\CurrentControlSet\\Enum\\DISPLAY\\%s\\%s\\Device Parameters\\EDID_OVERRIDE"), display_id, instance_id);

	if (count <= 0 || count >= STRING_SIZE)
		return FALSE;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
		return FALSE;

	size = sizeof buffer - sizeof *buffer;
	result = RegQueryValueEx(key, _T(""), NULL, NULL, (LPBYTE)buffer, &size);
	RegCloseKey(key);

	if (result != ERROR_SUCCESS)
		return FALSE;

	buffer[size / sizeof *buffer] = 0;

	if (*buffer < _T(' '))
		return FALSE;

	for (src = buffer, dst = name; *src >= _T(' '); src++, dst++)
		*dst = *src;

	*dst = 0;
	return TRUE;
}

/****************************************************************************/

BOOL load_driver_name(LPTSTR name, LPCTSTR display_id, LPCTSTR instance_id)
{
	int count;
	HKEY key;
	TCHAR subkey[STRING_SIZE];
	TCHAR buffer[STRING_SIZE];
	DWORD size;
	LONG result;
	LPTSTR src, dst;

	if (!name || !display_id || !instance_id)
		return FALSE;

	count = _sntprintf(subkey, STRING_SIZE, _T("SYSTEM\\CurrentControlSet\\Enum\\DISPLAY\\%s\\%s"), display_id, instance_id);

	if (count <= 0 || count >= STRING_SIZE)
		return FALSE;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
		return FALSE;

	size = sizeof buffer - sizeof *buffer;
	result = RegQueryValueEx(key, _T("DeviceDesc"), NULL, NULL, (LPBYTE)buffer, &size);
	RegCloseKey(key);

	if (result != ERROR_SUCCESS)
		return FALSE;

	buffer[size / sizeof *buffer] = 0;

	for (src = buffer; *src >= _T(' '); src++)
	{
		if (*src == _T(';'))
		{
			src++;
			break;
		}
	}

	if (*src < _T(' '))
		return FALSE;

	for (dst = name; *src >= _T(' '); src++, dst++)
		*dst = *src;

	*dst = 0;
	return TRUE;
}

/****************************************************************************/

BOOL load_display_instance(struct display_instance *display_instance, LPCTSTR display_id, LPCTSTR instance_id)
{
	BYTE override_data[DATA_SIZE];

	if (!display_instance || !display_id || !instance_id)
		return FALSE;

	if (!load_active_data(display_instance->data, display_id, instance_id))
		return FALSE;

	display_instance->active_status = load_active_status(display_id, instance_id);
	display_instance->override_status = load_override_data(override_data, display_id, instance_id);

	if (display_instance->override_status)
		memcpy(display_instance->data, override_data, DATA_SIZE);

	if (display_instance->active_status && display_instance->override_status)
		display_instance->active_override_status = TRUE;

	_tcscpy(display_instance->display_id, display_id);
	_tcscpy(display_instance->instance_id, instance_id);

	if (read_display_name(display_instance->name, display_instance->data))
		display_instance->name_status = TRUE;
	else if (load_backup_name(display_instance->name, display_id, instance_id))
		display_instance->name_status = TRUE;
	else if (!load_driver_name(display_instance->name, display_id, instance_id))
		_tcscpy(display_instance->name, display_id);
	else if (_tcscmp(display_instance->name, _T("Generic PnP Monitor")) == 0)
		_tcscpy(display_instance->name, display_id);

	return TRUE;
}

/****************************************************************************/

BOOL load_display(struct display_list *display_list, LPCTSTR display_id)
{
	int count;
	HKEY key;
	TCHAR subkey[STRING_SIZE];
	TCHAR instance_id[STRING_SIZE];
	DWORD index;
	DWORD size;
	struct display_instance *display_instance;

	if (!display_list || !display_id)
		return FALSE;

	count = _sntprintf(subkey, STRING_SIZE, _T("SYSTEM\\CurrentControlSet\\Enum\\DISPLAY\\%s"), display_id);

	if (count <= 0 || count >= STRING_SIZE)
		return FALSE;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey, 0, KEY_ENUMERATE_SUB_KEYS, &key) != ERROR_SUCCESS)
		return FALSE;

	for (index = 0; size = STRING_SIZE, RegEnumKeyEx(key, index, instance_id, &size, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; index++)
	{
		display_instance = new_display_instance();

		if (!load_display_instance(display_instance, display_id, instance_id))
		{
			free(display_instance);
			continue;
		}

		add_display_instance(display_list, display_instance);
	}

	RegCloseKey(key);
	return TRUE;
}

/****************************************************************************/

BOOL load_displays(struct display_list **display_list)
{
	int count;
	HKEY key;
	TCHAR subkey[STRING_SIZE];
	TCHAR display_id[STRING_SIZE];
	DWORD index;
	DWORD size;
	BOOL status = FALSE;

	if (!display_list)
		return FALSE;

	count = _sntprintf(subkey, STRING_SIZE, _T("SYSTEM\\CurrentControlSet\\Enum\\DISPLAY"));

	if (count <= 0 || count >= STRING_SIZE)
		return FALSE;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey, 0, KEY_ENUMERATE_SUB_KEYS, &key) != ERROR_SUCCESS)
		return FALSE;

	*display_list = new_display_list();

	for (index = 0; size = STRING_SIZE, RegEnumKeyEx(key, index, display_id, &size, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; index++)
		if (load_display(*display_list, display_id))
			status = TRUE;

	RegCloseKey(key);
	return status;
}

/****************************************************************************/

BOOL compare_displays(const struct display *display1, const struct display *display2)
{
	int result;

	if (display1->instance_list->active_count && !display2->instance_list->active_count)
		return TRUE;

	if (!display1->instance_list->active_count && display2->instance_list->active_count)
		return FALSE;

	result = _tcsicmp(display1->instance_list->first->name, display2->instance_list->first->name);

	if (result < 0)
		return TRUE;

	if (result == 0)
		if (_tcscmp(display1->instance_list->first->name, display2->instance_list->first->name) < 0)
			return TRUE;

	return FALSE;
}

/****************************************************************************/

BOOL sort_displays(struct display_list *display_list)
{
	struct display *cur, *src, *dst;

	if (!display_list)
		return FALSE;

	for (cur = display_list->first; cur && cur->next; )
	{
		if (compare_displays(cur, cur->next))
		{
			cur = cur->next;
			continue;
		}

		src = cur->next;
		cur->next = cur->next->next;

		if (compare_displays(src, display_list->first))
		{
			src->next = display_list->first;
			display_list->first = src;
			continue;
		}

		for (dst = display_list->first; dst && dst->next; dst = dst->next)
		{
			if (compare_displays(src, dst->next))
			{
				src->next = dst->next;
				dst->next = src;
				break;
			}
		}
	}

	display_list->last = cur;
	return TRUE;
}

/****************************************************************************/

const struct display *get_display(const struct display_list *display_list, HWND window, int item)
{
	int selection;
	int index;
	const struct display *display;

	if (!display_list)
		return NULL;

	selection = SendDlgItemMessage(window, item, CB_GETCURSEL, 0, 0);

	if (selection < 0)
		return NULL;

	for (index = 0, display = display_list->first; index < selection; index++, display = display->next)
		if (!display)
			return NULL;

	return display;
}

/****************************************************************************/

BOOL delete_override_data(LPCTSTR display_id, LPCTSTR instance_id)
{
	int count;
	HKEY key;
	TCHAR subkey[STRING_SIZE];

	if (!display_id || !instance_id)
		return FALSE;

	count = _sntprintf(subkey, STRING_SIZE, _T("SYSTEM\\CurrentControlSet\\Enum\\DISPLAY\\%s\\%s\\Device Parameters"), display_id, instance_id);

	if (count <= 0 || count >= STRING_SIZE)
		return FALSE;

	switch (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey, 0, KEY_SET_VALUE, &key))
	{
		case ERROR_SUCCESS:
			switch (RegDeleteKey(key, _T("EDID_OVERRIDE")))
			{
				case ERROR_SUCCESS:
				case ERROR_FILE_NOT_FOUND:
				case ERROR_PATH_NOT_FOUND:
					break;

				default:
					RegCloseKey(key);
					return FALSE;
			}

			RegCloseKey(key);
			break;

		case ERROR_FILE_NOT_FOUND:
		case ERROR_PATH_NOT_FOUND:
			break;

		default:
			return FALSE;
	}

	return TRUE;
}

/****************************************************************************/

BOOL delete_active_data(LPCTSTR display_id, LPCTSTR instance_id)
{
	int count;
	HKEY key;
	TCHAR subkey[STRING_SIZE];

	if (!display_id || !instance_id)
		return FALSE;

	count = _sntprintf(subkey, STRING_SIZE, _T("SYSTEM\\CurrentControlSet\\Enum\\DISPLAY\\%s\\%s\\Device Parameters"), display_id, instance_id);

	if (count <= 0 || count >= STRING_SIZE)
		return FALSE;

	switch (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey, 0, KEY_SET_VALUE, &key))
	{
		case ERROR_SUCCESS:
			switch (RegDeleteValue(key, _T("EDID")))
			{
				case ERROR_SUCCESS:
				case ERROR_FILE_NOT_FOUND:
				case ERROR_PATH_NOT_FOUND:
					break;

				default:
					RegCloseKey(key);
					return FALSE;
			}

			RegCloseKey(key);
			break;

		case ERROR_FILE_NOT_FOUND:
		case ERROR_PATH_NOT_FOUND:
			break;

		default:
			return FALSE;
	}

	return TRUE;
}

/****************************************************************************/

BOOL clear_established_resolutions(LPBYTE data)
{
	if (!data)
		return FALSE;

	memset(&data[35], 0, 3);
	return TRUE;
}

/****************************************************************************/

BOOL clear_standard_resolutions(LPBYTE data)
{
	if (!data)
		return FALSE;

	memset(&data[38], 1, 16);
	return TRUE;
}

/****************************************************************************/

BOOL clear_detailed_resolutions(LPBYTE data)
{
	if (!data)
		return FALSE;

	memset(&data[54], 0, 72);
	data[57] = 0x10;
	data[75] = 0x10;
	data[93] = 0x10;
	data[111] = 0x10;
	return TRUE;
}

/****************************************************************************/

BOOL set_extension_flag(LPBYTE data, int value)
{
	if (!data)
		return FALSE;

	data[126] = value;
	return TRUE;
}

/****************************************************************************/

BOOL clear_data(LPBYTE data)
{
	if (!data)
		return FALSE;

	memcpy(data, "\x00\xFF\xFF\xFF\xFF\xFF\xFF\x00", 8);
	memset(&data[12], 0, 4);

	if (data[18] != 1 || data[19] != 4)
	{
		data[18] = 1;
		data[19] = 3;

		if (data[20] & 128)
			data[20] = 128;
	}

	data[24] = data[24] & 0xFC | 0x02;
	clear_established_resolutions(data);
	clear_standard_resolutions(data);
	clear_detailed_resolutions(data);
	set_extension_flag(data, 1);
	memset(&data[128], 0, 128);
	memcpy(&data[128], "\x02\x03\x0C\x40\x23\x09\x07\x07\x83\x01\x00\x00", 12);
	return TRUE;
}

/****************************************************************************/

BOOL add_detailed_resolution(LPBYTE data, const struct resolution *resolution)
{
	int index;
	LPBYTE dst;
	int hsize;
	int vsize;
	int hblank;
	int vblank;
	int htotal;
	int vtotal;
	int pclock;

	if (!data)
		return FALSE;

	if (!resolution || !resolution->vrate)
		return FALSE;

	for (index = 0; index < 3; index++)
	{
		dst = &data[54 + index * 18];

		if (memcmp(dst, "\x00\x00\x00\x10\x00", 5) == 0)
			break;
	}

	if (index == 3)
	{
		for (index = 0; index < 6; index++)
		{
			dst = &data[140 + index * 18];

			if (memcmp(dst, "\x00\x00\x00\x00\x00", 5) == 0)
				break;
		}

		if (index == 6)
			return FALSE;
	}

	hsize = resolution->width >> 2;
	vsize = resolution->height >> 2;
	hblank = resolution->hfront + resolution->hsync + resolution->hback;
	vblank = resolution->vfront + resolution->vsync + resolution->vback;
	htotal = resolution->width + hblank;
	vtotal = resolution->height + vblank;

	if (resolution->vrate % 10000 == 4995)
		pclock = ((long long)resolution->vrate * htotal * vtotal - 1) / 100000000;
	else
		pclock = ((long long)resolution->vrate * htotal * vtotal + 99999999) / 100000000;

	dst[0] = pclock & 0xFF;
	dst[1] = pclock >> 8;
	dst[2] = resolution->width & 0xFF;
	dst[3] = hblank & 0xFF;
	dst[4] = ((resolution->width & 0x0F00) >> 4) | ((hblank & 0x0F00) >> 8);
	dst[5] = resolution->height & 0xFF;
	dst[6] = vblank & 0xFF;
	dst[7] = ((resolution->height & 0x0F00) >> 4) | ((vblank & 0x0F00) >> 8);
	dst[8] = resolution->hfront & 0xFF;
	dst[9] = resolution->hsync & 0xFF;
	dst[10] = ((resolution->vfront & 0x0F) << 4) | (resolution->vsync & 0x0F);
	dst[11] = ((resolution->hfront & 0x0300) >> 2) | ((resolution->hsync & 0x0300) >> 4) | ((resolution->vfront & 0x30) >> 2) | ((resolution->vsync & 0x30) >> 4);
	dst[12] = hsize & 0xFF;
	dst[13] = vsize & 0xFF;
	dst[14] = ((hsize & 0x0F00) >> 4) | ((vsize & 0x0F00) >> 8);
	dst[15] = 0x00;
	dst[16] = 0x00;
	dst[17] = (resolution->hpolarity ? 0x02 : 0x00) | (resolution->vpolarity ? 0x04 : 0x00) | 0x08 | 0x10;
	return TRUE;
}

/****************************************************************************/

int add_detailed_resolutions(LPBYTE data, const struct resolution **resolutions)
{
	int index;
	int result = 0;

	if (!data || !resolutions)
		return FALSE;

	for (index = 0; index < RESOLUTIONS; index++)
		if (add_detailed_resolution(data, resolutions[index]))
			result++;

	return result;
}

/****************************************************************************/

BOOL add_display_name(LPBYTE data, LPCTSTR name)
{
	int index;
	LPCTSTR src;
	LPBYTE dst;
	int count;

	if (!data)
		return FALSE;

	if (!name || *name < _T(' '))
		return FALSE;

	for (index = 0; index < 4; index++)
	{
		dst = &data[54 + index * 18];

		if (memcmp(dst, "\x00\x00\x00\x10\x00", 5) == 0)
		{
			dst[3] = '\xFC';

			for (src = name, dst += 5, count = 0; *src >= _T(' ') && count < 13; src++, dst++, count++)
				*dst = (BYTE)*src;

			if (count < 13)
				*dst = '\x0A';

			for (dst++, count++; count < 13; dst++, count++)
				*dst = '\x20';

			return TRUE;
		}
	}

	return FALSE;
}

/****************************************************************************/

BOOL calculate_checksum(LPBYTE data)
{
	int index;

	if (!data)
		return FALSE;

	data[127] = 0;
	data[255] = 0;

	for (index = 0; index < 127; index++)
	{
		data[127] -= data[index];
		data[255] -= data[index + 128];
	}

	return TRUE;
}

/****************************************************************************/

BOOL save_override_data(LPCBYTE data, LPCTSTR display_id, LPCTSTR instance_id)
{
	int count;
	HKEY key;
	TCHAR subkey[STRING_SIZE];
	LONG result;

	if (!data || !display_id || !instance_id)
		return FALSE;

	count = _sntprintf(subkey, STRING_SIZE, _T("SYSTEM\\CurrentControlSet\\Enum\\DISPLAY\\%s\\%s\\Device Parameters\\EDID_OVERRIDE"), display_id, instance_id);

	if (count <= 0 || count >= STRING_SIZE)
		return FALSE;

	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, subkey, 0, NULL, 0, KEY_SET_VALUE, NULL, &key, NULL) != ERROR_SUCCESS)
		return FALSE;

	result = RegSetValueEx(key, _T("0"), 0, REG_BINARY, data, 128);

	if (result == ERROR_SUCCESS)
		result = RegSetValueEx(key, _T("1"), 0, REG_BINARY, &data[128], 128);

	RegCloseKey(key);

	if (result != ERROR_SUCCESS)
		return FALSE;

	return TRUE;
}

/****************************************************************************/

BOOL save_backup_name(LPCTSTR name, LPCTSTR display_id, LPCTSTR instance_id)
{
	int count;
	HKEY key;
	TCHAR subkey[STRING_SIZE];
	LONG result;

	if (!name || !display_id || !instance_id)
		return FALSE;

	count = _sntprintf(subkey, STRING_SIZE, _T("SYSTEM\\CurrentControlSet\\Enum\\DISPLAY\\%s\\%s\\Device Parameters\\EDID_OVERRIDE"), display_id, instance_id);

	if (count <= 0 || count >= STRING_SIZE)
		return FALSE;

	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, subkey, 0, NULL, 0, KEY_SET_VALUE, NULL, &key, NULL) != ERROR_SUCCESS)
		return FALSE;

	result = RegSetValueEx(key, _T(""), 0, REG_SZ, (LPCBYTE)name, _tcslen(name) * sizeof *name + sizeof *name);
	RegCloseKey(key);

	if (result != ERROR_SUCCESS)
		return FALSE;

	return TRUE;
}

/****************************************************************************/

BOOL save_display_instance(struct display_instance *display_instance, const struct resolution **resolutions)
{
	if (!display_instance || !resolutions)
		return FALSE;

	if (!resolutions[0])
	{
		if (!delete_override_data(display_instance->display_id, display_instance->instance_id))
			return FALSE;

		//delete_active_data(display_instance->display_id, display_instance->instance_id);
		return TRUE;
	}

	clear_data(display_instance->data);
	//set_display_id(display_instance->data, display_instance->display_id);
	add_detailed_resolutions(display_instance->data, resolutions);

	if (display_instance->name_status)
		add_display_name(display_instance->data, display_instance->name);

	calculate_checksum(display_instance->data);

	if (!save_override_data(display_instance->data, display_instance->display_id, display_instance->instance_id))
		return FALSE;

	if (display_instance->name_status)
		save_backup_name(display_instance->name, display_instance->display_id, display_instance->instance_id);

	return TRUE;
}

/****************************************************************************/

BOOL save_display(const struct display *display, const struct resolution **resolutions)
{
	struct display_instance *display_instance;
	BOOL status = TRUE;

	if (!display || !resolutions)
		return FALSE;

	for (display_instance = display->instance_list->first; display_instance; display_instance = display_instance->next)
		if (!save_display_instance(display_instance, resolutions))
			status = FALSE;

	return status;
}

/****************************************************************************/

int test_device_frequencies(DISPLAY_DEVICE device, const int *frequencies)
{
	DEVMODE mode;
	int index;

	if (!frequencies)
		return FALSE;

	mode.dmSize = sizeof mode;

	if (!EnumDisplaySettings(device.DeviceName, ENUM_CURRENT_SETTINGS, &mode))
		return FALSE;

	mode.dmDisplayFlags = 0;

	for (index = 0; frequencies[index]; index++)
	{
		mode.dmDisplayFrequency = frequencies[index];

		if (ChangeDisplaySettingsEx(device.DeviceName, &mode, NULL, CDS_TEST, NULL) == DISP_CHANGE_SUCCESSFUL)
			return frequencies[index];
	}

	return FALSE;
}

/****************************************************************************/

BOOL test_frequencies(const int *frequencies)
{
	int index;
	DISPLAY_DEVICE device;

	if (!frequencies)
		return FALSE;

	for (index = 0; device.cb = sizeof device, EnumDisplayDevices(NULL, index, &device, 0); index++)
		if (test_device_frequencies(device, frequencies))
			return TRUE;

	return FALSE;
}

/* Main *********************************************************************/

BOOL CALLBACK main_window(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	static struct display_list *display_list;
	static BOOL reset_only;
	HFONT font;

	switch (message)
	{
		case WM_INITDIALOG:
			if (!load_displays(&display_list))
			{
				MessageBox(NULL, _T("Failed to load displays from the registry."), STROBELIGHT_VERSION, MB_ICONERROR);
				PostQuitMessage(1);
				return TRUE;
			}

			if (display_list->count == 0)
			{
				MessageBox(NULL, _T("No displays found in the registry."), STROBELIGHT_VERSION, MB_ICONERROR);
				PostQuitMessage(1);
				return TRUE;
			}

			sort_displays(display_list);
			list_displays(display_list, window, IDC_DISPLAY, 0);

			if (GetSystemMetrics(SM_CLEANBOOT))
			{
				MessageBox(NULL, _T("Safe mode detected.\nOnly reset will be available."), STROBELIGHT_VERSION, MB_ICONWARNING);
				reset_only = TRUE;
			}
			else if (!test_frequencies(frequencies))
			{
				MessageBox(NULL, _T("No 120 Hz displays found.\nOnly reset will be available."), STROBELIGHT_VERSION, MB_ICONWARNING);
				reset_only = TRUE;
			}
			else
			{
				CheckDlgButton(window, IDC_120_STROBED, TRUE);
				CheckDlgButton(window, IDC_120, TRUE);
			}

			font = CreateFont(-MulDiv(18, GetDeviceCaps(GetDC(window), LOGPIXELSY), 72), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Segoe UI"));
			SendDlgItemMessage(window, IDC_TITLE, WM_SETFONT, (WPARAM)font, TRUE);
			ShowWindow(window, SW_SHOW);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				default:
				{
					BOOL enabled;
					int index;

					if (IsDlgButtonChecked(window, IDC_RESET))
					{
						if (LOWORD(wParam) == IDC_RESET)
							SetDlgItemText(window, IDOK, _T("Reset this display"));

						EnableWindow(GetDlgItem(window, IDOK), TRUE);
						enabled = FALSE;
					}
					else
					{
						if (LOWORD(wParam) == IDC_RESET)
							SetDlgItemText(window, IDOK, _T("Install refresh rates"));

						if (reset_only)
						{
							EnableWindow(GetDlgItem(window, IDOK), FALSE);
							enabled = FALSE;
						}
						else
						{
							enabled = FALSE;

							for (index = 0; index < STROBED_RESOLUTIONS; index++)
								if (IsDlgButtonChecked(window, IDC_RESOLUTIONS + index))
									enabled = TRUE;

							EnableWindow(GetDlgItem(window, IDOK), enabled);
							enabled = TRUE;
						}
					}

					for (index = 0; index < RESOLUTIONS; index++)
						EnableWindow(GetDlgItem(window, IDC_RESOLUTIONS + index), enabled);

					return TRUE;
				}

				case IDOK:
				{
					const struct display *display;
					const struct resolution *resolutions[RESOLUTIONS] = {0};
					int index;
					int count = 0;

					display = get_display(display_list, window, IDC_DISPLAY);

					if (!IsDlgButtonChecked(window, IDC_RESET))
						for (index = 0; index < RESOLUTIONS; index++)
							if (IsDlgButtonChecked(window, IDC_RESOLUTIONS + index))
								resolutions[count++] = &resolution_list[index];

					ShowWindow(window, SW_HIDE);

					if (!save_display(display, resolutions))
					{
						if (resolutions[0])
							MessageBox(NULL, _T("Failed to install refresh rates.\nTry running as administrator."), STROBELIGHT_VERSION, MB_ICONERROR);
						else
							MessageBox(NULL, _T("Failed to uninstall refresh rates.\nTry running as administrator."), STROBELIGHT_VERSION, MB_ICONERROR);

						PostQuitMessage(1);
						return TRUE;
					}

					if (resolutions[0])
						MessageBox(NULL, _T("Refresh rates installed successfully.\nChanges will take effect after rebooting."), STROBELIGHT_VERSION, MB_ICONINFORMATION);
					else
						MessageBox(NULL, _T("Refresh rates uninstalled successfully.\nChanges will take effect after rebooting."), STROBELIGHT_VERSION, MB_ICONINFORMATION);

					PostQuitMessage(0);
					return TRUE;
				}

				case IDCANCEL:
				{
					PostQuitMessage(0);
					return TRUE;
				}
			}
	}

	return FALSE;
}

/****************************************************************************/

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	HWND window;
	MSG message = {0};

	DeleteFile(_T("strobelight.exe:Zone.Identifier"));
	CreateMutex(NULL, TRUE, _T("7dd2ed38018cfa8cbf8924d922125fee"));

	if (GetLastError() == ERROR_ALREADY_EXISTS)
		return 1;

	window = CreateDialog(hInstance, _T("IDD_MAIN"), NULL, main_window);

	if (!window)
		return 1;

	while (GetMessage(&message, NULL, 0, 0) > 0)
	{
		if (!IsDialogMessage(window, &message))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	return message.wParam;
}
