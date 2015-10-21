/*
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
			for ( int j=0; j<root.size(); j++ ) {
				Json::Value single_unit = root[j];
				cout<<single_unit<<endl;
				unsigned long long int time_stamp = single_unit["time_stamp"].asUInt64();
				int offset_x = single_unit["x"].asInt();
				int offset_y = single_unit["y"].asInt();
				int height = single_unit["height"].asInt();
				int width = single_unit["width"].asInt();
				int people_id = single_unit["people_id"].asInt();

				char src_loc[1024];
				sprintf(src_loc, "%s/%lld.jpg", argv[1], time_stamp);
				Mat src_img = imread(src_loc);
				if (!src_img.empty()) {
					int w_height = src_img.rows;
					int w_width = src_img.cols;
					Mat sub;
					sub.create(height, width, CV_8UC3);
					for ( int y = 0; y<height; y++ ) {
						for ( int x=0; x<width; x++ ) {
							sub.data[3*(y*width+x) + 0 ] = src_img.data[3*( (y+offset_y)*w_width + (x+offset_x)) + 0];
							sub.data[3*(y*width+x) + 1 ] = src_img.data[3*( (y+offset_y)*w_width + (x+offset_x)) + 1];
							sub.data[3*(y*width+x) + 2 ] = src_img.data[3*( (y+offset_y)*w_width + (x+offset_x)) + 2];
						}
					}

					char sub_loc[1024];
					sprintf(sub_loc, "%s/sub_%d.jpg", argv[1], parse_num++);
					imwrite(sub_loc, sub);
				}
			}
		}
		ifs.close();
	}

	return 0;
}
