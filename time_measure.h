#pragma once

#include <windows.h>
#include <tchar.h>

namespace measure
{

class time
{
public:
	time()
	{
		::ZeroMemory(&m_start, sizeof(LARGE_INTEGER));
		::ZeroMemory(&m_stop, sizeof(LARGE_INTEGER));
		::ZeroMemory(&m_freq, sizeof(LARGE_INTEGER));
	}
	time(const wchar_t* measure_id)
	{
		::ZeroMemory(&m_start, sizeof(LARGE_INTEGER));
		::ZeroMemory(&m_stop, sizeof(LARGE_INTEGER));
		::ZeroMemory(&m_freq, sizeof(LARGE_INTEGER));
		start_measure(measure_id);
	}

	~time()
	{
		if(m_measuring) {
			end_measure();
		}
	}

	void start_measure(const wchar_t* measure_id)
	{
		m_measuring = true;
		size_t id_len = wcslen(measure_id);
		wcsncpy_s(m_measure_id, _countof(m_measure_id), measure_id, id_len);
		::QueryPerformanceFrequency(&m_freq);
		::QueryPerformanceCounter(&m_start);
	}

	void end_measure(std::function<void(const wchar_t* id, double dDiff)> log_func = nullptr, bool reset_measure = false, const wchar_t* measure_id = L"")
	{
		::QueryPerformanceCounter(&m_stop);
		m_measuring = false;

		double dDiff;
		dDiff = double(m_stop.QuadPart - m_start.QuadPart);
		dDiff /= m_freq.QuadPart;

		if(log_func) {
			log_func(m_measure_id, dDiff);
		}

		if(reset_measure) {
			start_measure(measure_id);
		}
	}

private:
	bool          m_measuring = false;
	LARGE_INTEGER m_start, m_stop, m_freq;
	wchar_t       m_measure_id[64] = { 0 };
};

}; // End of namespace measure