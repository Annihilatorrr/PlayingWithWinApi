// WinApi.App.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <format>
#include <iomanip>
#include <iostream>
#include <ranges>
#include "../WinApiLib/winprocessservice.h"
#include <algorithm>

void printProcesses(const ProcessInfo& pi, bool recursively = false, int level = 0)
{
	std::wcout
		<< std::setw(level)
		<< std::right
		<< L"-"
		<< std::left
		<< std::setw(6) << pi.id << " "
		<< std::setfill(L'.')
		<< std::setw(50 + level) << pi.name
		<< std::setfill(L' ');
	std::wcout
		<< std::setw(16) << std::format(L"{} K", pi.extendedInfo.memoryInfo.WorkingSetSize)
		<< std::setw(16) << std::format(L"{} K", pi.extendedInfo.memoryInfo.WorkingSetSize)
		<< std::setw(16) << std::format(L"{} K", pi.extendedInfo.memoryInfo.PageFileUsage)
		<< std::setw(16) << pi.perfData.frequency100Ns
		<< std::setw(16) << pi.perfData.percentProcessorTime
		<< std::endl;

	/*if (recursively)
	{
		for (const auto& child : pi.childProcesses)
		{
			printProcesses(child, recursively, level + 2);
		}
	}*/
}

int main()
{
	while (true)
	{
		WA::WinProcessService pr;


		const auto processes = pr.getProcessTreeByCom();
		auto range{ processes | std::views::values };
		std::vector sortedProcesses(range.begin(), range.end());
		std::ranges::sort(sortedProcesses,
			[](const auto& a, const auto& b)
			{
				return a.extendedInfo.memoryInfo.WorkingSetSize > b.extendedInfo.memoryInfo.WorkingSetSize;
			});

		/*std::wcout
			<< std::left
			<< std::setw(6) << "PID" << " "
			<< std::setw(50) << "Process"
			<< std::setw(50) << "CPU"
			<< std::setw(16) << "Working Set"
			<< std::setw(16) << "Private bytes"
			<< std::setw(16) << "Freq"
			<< std::setw(16) << "Percentage"
			<< std::endl;*/

		//std::wcout << std::setfill(L'-') << std::setw(100) << "-" << std::endl;

		for (const auto& process : sortedProcesses)
		{
			if (process.name == L"ProcessViewer.exe")
				printProcesses(process, true);
		}
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
