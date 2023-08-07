#pragma once
#ifndef _LOCAL_STORAGE_H_
#define _LOCAL_STORAGE_H_


class savedData {

	savedData();
	~savedData();
	savedData& operator=(const savedData&) = delete;
public:
	static savedData& getInstance();
	void loadData();
	/*void grabfromLoadLocal(const std::filesystem::path());
	void grabfromLoadLocal(const  std::filesystem::path& a);*/
};

#endif

