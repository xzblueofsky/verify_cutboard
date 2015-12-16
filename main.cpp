/*
 *
 *
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  verify cutboard image
 *
 *        Version:  1.0
 *        Created:  2015年10月19日 20时29分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  XinZhao (zx), xinzhao@deepglint.com
 *        Company:  DeepGlint
 *
 * =====================================================================================
 */


#include "jsoncpp/json/json.h"
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <fstream>

using namespace std;
using namespace cv;

/*function... might want it in some class?*/
int getdir (string dir, vector<string> &files)
{
		    DIR *dp;
		    struct dirent *dirp;
		    if((dp  = opendir(dir.c_str())) == NULL) {
		        cout << "Error(" << errno << ") opening " << dir << endl;
		        return errno;
		    }

		    while ((dirp = readdir(dp)) != NULL) {
				if (string(dirp->d_name)=="." || string(dirp->d_name)=="..") {
					continue;
				}
		        files.push_back(dir + "/" +string(dirp->d_name));
		    }
		    closedir(dp);
		    return 0;
}

int main( int argc, char* argv[]) {
	printf("hello world\n");
	if (argc!=2) {
		printf("Usage: ./a.out <path>\n");
		return -1;
	}
	
	// get whole file list
	vector<string> files = vector<string>();

	getdir(argv[1],files);

	if (files.size()==0) {
		printf("File read error: %s\n", argv[1]);
		return -1;
	}

	for (unsigned int i = 0;i < files.size();i++) {
	    cout << files[i] << endl;
    }

	printf("========================\n");
	// get json file list 
	vector<string> json_files;
	for ( int i=0; i<files.size(); i++ ) {
		if (files[i].find(".json")!=string::npos) {
			json_files.push_back(files[i]);
		}
	}
	printf("========================\n");

	// parse json file
	static int parse_num = 0;
	for ( int i=0; i<json_files.size(); i++ ) {
		cout<<json_files[i]<<endl;
		Json::Reader reader;
		Json::Value root(Json::arrayValue);

		ifstream ifs;
		ifs.open(json_files[i].c_str(), ios::binary);

		if (reader.parse(ifs, root)) {
			// parse each json unit
			cout<<root<<endl;
			Json::Value cutboard_box = root["CutboardBox"];
			cout<<cutboard_box<<endl;
			if(cutboard_box.empty()) {
				ifs.close();
				continue;
			}
			int offset_x = root["CutboardBox"][0].asInt();
			int offset_y = root["CutboardBox"][1].asInt();
			int height = root["CutboardBox"][2].asInt();
			int width = root["CutboardBox"][3].asInt();
			printf("x = %d, y = %d, height = %d, width = %d\n", offset_x, offset_y, height, width);
			unsigned long long int start_time = root["StartTime"].asUInt64();
			cout<<root["CutboardTimeOffset"]<<endl;
			int time_offset = root["CutboardTimeOffset"][0].asInt();
			unsigned long long int time_stamp = start_time - time_offset;
			char img_loc[1024];
			sprintf(img_loc, "%lld.jpg", time_stamp);
			Mat image = imread(img_loc);
			rectangle(image, Point(offset_x, offset_y), Point(offset_x + width, offset_y + height), Scalar(255, 0, 0), 2);
			imshow("image", image);
			waitKey(0);
		}
		ifs.close();
	}

	return 0;
}
