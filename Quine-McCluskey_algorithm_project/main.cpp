#define _CRT_SECURE_NO_WARNINGS
#include <iostream>//기본 입출력 헤더
#include <fstream>//파일 입출력 헤더
#include <string>//스트링 헤더
#include <vector>//벡터 헤더
#include <utility>//페어 헤더
#include <algorithm>//알고리즘 헤더
using namespace std;
class group
{
public:
	vector<pair<string, bool>> minterm;
	int data;
	group() { this->data = 0; }
	~group() { minterm.clear(); }
};
bool read_data();//V 파일 불러오는 함수, 반환값으로 호출 성공 여부 판단
int cnt_ones(string arr);//비트 안에 1의 개수를 세고 반환하는 함수
void set_term(pair<string, bool> new_data);//V 입력받은 항을 tmp_list에 넣는 함수
bool isprimeterm(string a, string b);//V 두 term이 간소화 가능한 항인지 판별하는 함수 되면 true, 안되면 false
pair<string, bool> make_minterm(string a, string b);//V 간소화된 식을 만들어주는 함수
void check_overlap();//V 간소화 식을 tmp에 저장한 후 중복을 확인하고 제거하는 함수
void change_list();//V list와 tmp_list를 교환하는 함수
void elimination();//V 간소화하는 비교 과정 함수 
bool check_condition();//V 간소화 과정의 조건을 체크하는 함수, 간소화가 끝났으면 1, 아니면 0
void PI_extraction();//별표인 남은 PI를 PI 변수에 저장하는 함수
void Petrick_Method();//epi를 뽑아내는 알고리즘을 실행하여 epi를 저장하는 함수
bool iscovercorrect(string a, string b);//minterm을 포함하는 PI인지 확인하는 함수
void save_EPI();//파일에 EPI를 저장하는 함수

int n, n_1;//비트수를 입력받을 변수, 비트수+1
group* list = NULL, * tmp_list = NULL;//원본 리스트를 저장하는 변수, 새로운 column을 비교 후 생성 시 저장하는 임시 변수
vector<string> minterm, PI;//파일에서 불러온 minterm들을 전부 저장하는 배열 변수, 비교 후 남은 prime implicant를 저장할 변수
vector<string> EPI;//EPI를 담는 변수

int main(void)
{
	if (read_data()) return 1;//파일 호출 실패시 프로그램 종료
	while (1)
	{
		elimination();//간소화 과정 함수 호출
		check_overlap();//중복 제거 함수 호출
		PI_extraction();//list에 남은 항 PI에 저장 함수 호출
		if (check_condition())	break;//종료 조건 설정 함수
		change_list();//list와 tmp_list 스왑 함수 호출
	}
	delete[] list;//할당 해제
	delete[] tmp_list;//할당 해제
	Petrick_Method();//주항 차트로 패트릭의 방법 함수 호출
	save_EPI();//파일에 EPI 저장함수 호출
	return 0;
}

void elimination()
{
	for (int i = 1; i < n_1; i++)//그룹 수 만큼
	{
		int size = static_cast<int> (list[i - 1].minterm.size());
		for (int j = 0; j < size; j++)//기준 그룹(앞의 그룹)의 데이터 수만큼
		{
			for (int k = 0; k < list[i].minterm.size(); k++)//비교 그룹(뒤의 그룹)의 데이터 수만큼
			{
				if (isprimeterm(list[i - 1].minterm[j].first, list[i].minterm[k].first))//둘이 간소화 가능한지 체크
				{
					set_term(make_minterm(list[i - 1].minterm[j].first, list[i].minterm[k].first));//간소화 항 만든 후 tmp에 저장
					list[i - 1].minterm[j].second = true;//체크표시
					list[i].minterm[k].second = true;//체크표시
				}
			}
		}
	}
}
void PI_extraction()
{
	for (int i = 0; i < n_1; i++)//그룹별로
	{
		for (int j = 0; j < list[i].minterm.size(); j++)//한 그룹 순회
		{
			if (list[i].minterm[j].second == false)//별표인 항 체크
				PI.push_back((list[i].minterm[j].first));//PI에 저장
		}
	}
}
bool isprimeterm(string a, string b)
{
	int len = static_cast<int>(a.length());//어차피 길이는 같으므로 하나의 길이를 불러옴
	int cnt = 0;//다른 자리를 판별하는 변수
	for (int i = 0; i < len; i++)
	{
		if (cnt > 1)	return false;//2개 이상 다르면 false
		if (abs(a[i] - b[i]) == 1)	cnt++;// 둘이 0,1이면 cnt 증가
		else if (abs(a[i] - b[i]) == 3 || abs(a[i] - b[i]) == 4)	return false;//하이픈과 숫자가 오면 false
	}
	return true;//둘이 하나만 다르면 true
}
pair<string, bool> make_minterm(string a, string b)
{
	pair<string, bool> new_term = make_pair(a, false);//일단 둘 중 하나의 string으로 대입
	int len = static_cast<int> (a.length());//하나의 길이를 불러옴
	for (int i = 0; i < len; i++)
	{
		if (a[i] != b[i])  new_term.first[i] = '-';//하나만 다른 부분인 곳을 하이픈으로 만듬
	}
	return new_term;//만든 항을 반환
}
void set_term(pair<string, bool> new_data)
{
	if (tmp_list == NULL)//tmp_list가 할당되어있지 않으면 그룹 배열을 할당함
	{
		tmp_list = new group[n_1];
		for (int i = 0; i < n_1; i++)
			tmp_list[i].data = i;
	}
	int cnt = cnt_ones(new_data.first);//입력받은 데이터의 1의 개수를 셈
	tmp_list[cnt].minterm.push_back(make_pair(new_data.first, new_data.second));//해당 그룹에 넣음
}
void check_overlap()
{
	if (tmp_list != NULL)//tmp참조 오류 발생 방지 제어문
	{
		for (int i = 0; i < n_1; i++)//unique를 이용하여 중복을 vector의 뒤로 이동시킴, 이후 erase로 제거
			tmp_list[i].minterm.erase(unique(tmp_list[i].minterm.begin(), tmp_list[i].minterm.end()), tmp_list[i].minterm.end());
	}
}
bool check_condition()//간소화 과정의 조건을 체크하는 함수, 간소화가 끝났으면 1, 아니면 0
{
	for (int i = 0; i < n_1; i++)//그룹 수 만큼
	{
		int size = static_cast<int> (list[i].minterm.size());
		for (int j = 0; j < size; j++)//각 그룹마다
		{
			if (list[i].minterm[j].second == true)  return false;//체크 표시가 있다느 것은 tmp 컬럼에 있다는 뜻이므로 false
		}
	}
	return true;//하나도 체크 표시가 없을 경우 종료를 위해 true 반환
}
void Petrick_Method()
{
	int rows = PI.size();//table 행 길이
	int cols = minterm.size();//table 열 길이
	int** table = new int* [rows];//PI 개수만큼 행 할당
	for (int i = 0; i < rows; i++)//minterm 개수만큼 열 할당
		table[i] = new int[cols];
	for (int i = 0; i < rows; i++)//각 행마다
	{
		for (int j = 0; j < cols; j++)//각 열마다
		{
			if (iscovercorrect(minterm[j], PI[i]))	table[i][j] = 1;//minterm을 포함하는 PI인지 체크하여 맞으면 1
			else table[i][j] = 0; // 아니면 0 반환
		}
	}
	for (int i = 0; i < cols; i++)
	{
		int cnt = 0;//1 개수 셀 변수
		for (int j = 0; j < rows; j++)
		{
			if (table[j][i] == 1)//1 찾을시 cnt 증가
				cnt++;
		}
		if (cnt == 1)//x가 1기개면
		{
			for (int j = 0; j < rows; j++)//행을 돌면서 1인 곳을 다시 찾음
			{
				if (table[j][i] == 1)//1인 곳을 찾음
				{
					EPI.push_back(PI[j]);//그때의 PI를 EPI로 채택, 저장
					for (int k = 0; k < cols; k++)//다른 열에 있는 거 찾음
					{
						if (table[j][k] == 1)//1인거 찾음
						{
							for (int p = 0; p < rows; p++)//1인 항의 열을 다 0으로 만듬
								table[p][k] = 0;
						}
					}
					break;
				}
			}
		}
	}
	vector<int> cover(rows);
	while(1)
	{
		int stop = 0;//loop stop을 위한 제어 조건 체크
		for (int i = 0; i < rows; i++)//table의 모든 칸을 체크하여 1인 칸 확인
		{
			for (int j = 0; j < cols; j++)
			{
				if (table[i][j] == 1)
					stop++;
			}
		}
		if (!stop) break;//1인 칸이 아무곳도 없으면 stop
		for (int i = 0; i < rows; i++)//각 행마다
		{
			int cnt = 0;
			for (int j = 0; j < cols; j++)//각 열마다
			{
				if (table[i][j] == 1)//1인 곳은 cnt 증가
					cnt++;
			}
			cover.push_back(cnt);//cover vector에 PI가 포함하는 minterm 개수 합 저장
		}
		int max_id = 0;//최댓값 인덱스 저장, 첫번쨰 인덱스(0)으로 초기화
		for (int i = 0; i < rows; i++)//cover vector 순회
		{
			if (cover[max_id] < cover[i]) max_id = i;//최댓값 저장 조건 제어, 최댓값 인덱스 저장
		}
		EPI.push_back(PI[max_id]);//최대 개수 커버 PI를 EPI로 채택
		for (int i = 0; i < cols; i++)//PI 해당 열에
		{
			if (table[max_id][i] == 1)//1인 부분을 0으로 바꿈
			{
				for (int j = 0; j < rows; j++)//한 열을 모두 0으로 바꿈(커버하므로)
					table[j][i] = 0;
			}
		}
	}
	for (int i = 0; i < rows; i++)//메모리 해제
		delete[] table[i];//메모리 해제
	delete[] table;//메모리 해제
}
bool iscovercorrect(string a, string b)
{
	int len = static_cast<int>(a.length());
	for (int i = 0; i < len; i++)
	{
		if (a[i] != b[i] && a[i] != '-' && b[i] != '-')//포함하는 경우가 아니므로 false 반환
			return false;
	}
	return true;//포함하느 경우이므로 true 반환
}
void change_list()
{
	delete[] list;//list 할당 해제
	list = tmp_list;//tmp_list를 list로 넘김
	tmp_list = NULL;//tmp배열은 NULL로 만듬
}
int cnt_ones(string arr)//비트 안에 1의 개수를 count하고 반환하는 함수
{
	int cnt = 0;//인덱스 변수, 1의 개수 변수
	for (int i = 0; i < arr.length(); i++)
		if (arr[i] == '1')	cnt++; // 1일 때 cnt를 증가함
	return cnt;
}
bool read_data()
{
	//////data define///////
	string line;
	char c;
	ifstream read_file;
	read_file.open("input_minterm.txt");//파일 열기
	if (read_file.fail())	return 1;//파일 호출 실패시 1 반환
	std::getline(read_file, line);//숫자 첫줄 받아옴
	n = stoi(line);//문자열인 숫자를 int로 바꿈
	n_1 = n + 1;//n_1의 값을 정함
	list = new group[n_1];//비트수 +1 (0~n까지)만큼 그룹을 만듬
	for (int i = 0; i < n_1; i++)//처음 리스트의 1 개수 데이터를 각 리스트 인덱스로 저장
		list[i].data = i;
	while (!read_file.eof())//////파일에서 그룹에 저장하는 부분//////////
	{
		//앞의 m과 스페이스 시퀀스를 제거함
		read_file.get(c);
		read_file.get(c);
		std::getline(read_file, line);//한줄 받아옴
		int tmp = cnt_ones(line);//1의 개수를 셈
		list[tmp].minterm.push_back(make_pair(line, false));//1개수로 그룹으로 나눠 저장함
		minterm.push_back(line);//minterm의 항으로 저장함
	}
	read_file.close();//파일 닫기
	return 0;
}
void save_EPI()
{
	ofstream write_file;//파일 출력 클래스 선언
	write_file.open("result.txt");//파일 열기
	for (int i = 0; i < EPI.size(); i++)  write_file << EPI[i] << "\n";//파일에 한줄씩 EPI를 출력
	write_file.close();//파일 닫기
}