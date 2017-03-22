#include "structs.h"
bool check_over(int a,int b,int ans){
	if(a<0&&b<0){
		if(ans>0)
			return true;
	}
	else if(a>0&&b>0){
		if(ans<0)
			return true;
	}
	else return false;
	return false;
}
