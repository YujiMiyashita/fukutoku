#include <stdio.h>
#include <stdlib.h>

#define EPS    1E-6 // 無限小
#define LARGE  1E+30 // 無限大
#define MMAX   20 // 行数の上限
#define NMAX   100 // 列数の上限

float variable[MMAX + 1][NMAX + 1],                        /*条件式の係数*/
  purpose[NMAX + 1],                                    /*目的関数の係数*/
  variable_matrix[MMAX + 1][MMAX + 1],                                /*変数行列*/
  pivotcolumn[MMAX + 1];                              /*ピポット列*/
int num_conditions, num_variables, // 条件の数, 変数の数
  variable_and_minus_slug,                      // 変数の数 + 負のスラック変数の数
  variable_and_slug,                // 変数の数 + 全スラッグ変数の数
  variable_and_slug_and_artificial,                // 変数の数 + 全スラッグ変数の数 + 全人為変数の数
  num_right_col,  // 一番右の列の番号
  col[MMAX + 1],      // 各行の基底変数の番号
  row[NMAX + 2 * MMAX + 1],  // その列が基底なら対応する条件の番号
  nonzero_row[NMAX + 2 * MMAX + 1]; // スラック・人為変数の０でない行
char inequality[MMAX + 1]; // 演算子

void error(char *message);
double getnum(void);
void readdata(void);
void prepare(void);
double tableau(int row_num,int col_num);
void writetableau (int row_pivot ,int col_pivot);
void pivot(int row_pivot,int col_pivot);
void minimize(void); //OK
void phase1(void); //OK
void phase2(void); //OK
void report(void); //OK

int main() {
  // 条件/変数/目的関数/制約条件の係数を読み込み
  readdata();
  // スラッグ変数＆人為変数のセッティング
  prepare();
  // 人為変数が存在していれば実行
  if (variable_and_slug_and_artificial != variable_and_slug)
    phase1();
  // スラッグ変数を元にピポッド演算
  phase2();
  // 結果をコンソールに出力
  report();
  return EXIT_SUCCESS;
}


// エラーを出力する関数
void error(char *message) {
  fprintf(stderr, "\n%s\n", message);
  exit(EXIT_FAILURE);
}

// 標準入力
double getnum(void) {
  int data;
  double num;

  while ((data = scanf("%lf", &num)) != 1) {
    if (data == EOF) {
      error("入力エラー");
    }
    scanf("%*[^\n]");
  }
  return num;
}

// データ読み込み
void readdata(void) {
  int count_1, count_2;
  char data[2];

  num_conditions = (int)getnum(); // 条件式の数を入力
  num_variables = (int)getnum(); // 変数の数を入力

  // 条件の数または変数の数が範囲外の時はエラーを吐く
  if (num_conditions < 1 || num_conditions > MMAX || num_variables < 1 || num_variables > NMAX) {
    error("条件の数mまたは変数の数nが範囲外です。");
  }

  // 目的関数 + 制約条件の式を入力
  for (count_1 = 1; count_1 <= num_variables; count_1++) {
    purpose[count_1] = getnum();
  }
  purpose[0] = -getnum(); //目的関数の値の符号を逆にする

  for (count_1 = 1; count_1 <= num_conditions; count_1++) {
    for (count_2 = 1; count_2 <= num_variables; count_2++) {
      variable[count_1][count_2] = getnum();
    }
    if (scanf(" %1[><=]", data) != 1) {
      error("入力エラー");
    }
    inequality[count_1] = data[0];
    variable[count_1][0] = getnum();

    if (variable[count_1][0] < 0) {
      if (inequality[count_1] == '>')
        inequality[count_1] = '<';
      else if (inequality[count_1] == '<')
        inequality[count_1] = '>';

      for (count_2 = 0; count_2 <= num_variables; count_2++) {
        variable[count_1][count_2] = -variable[count_1][count_2];
      }
    }
    else if (variable[count_1][0] == 0 && inequality[count_1] == '>') {
      inequality[count_1] = '<';
      for (count_2 = 1; count_2 <= num_variables; count_2++) {
        variable[count_1][count_2] = -variable[count_1][count_2];
      }
    }
  }
}

void prepare(void) {
  int count;

  variable_and_minus_slug = num_variables;
  for (count = 1; count <= num_conditions; count++) {
    if (inequality[count] == '>') {       /*係数が-1のスラック変数*/
      variable_and_minus_slug++;
      nonzero_row[variable_and_minus_slug] = count;
    }
  }
  variable_and_slug = variable_and_minus_slug;
  for (count = 1; count <= num_conditions; count++) {
    if (inequality[count] == '<') {       /*係数が+1のスラック変数*/
      variable_and_slug++; col[count] =variable_and_slug;
      nonzero_row[variable_and_slug]=row[variable_and_slug]=count;
    }
  }
  variable_and_slug_and_artificial = variable_and_slug;
  for (count = 1; count <= num_conditions; count++) {
    if (inequality[count] != '<') {    /*人為変数*/
      variable_and_slug_and_artificial++; col[count] = variable_and_slug_and_artificial;
      nonzero_row[variable_and_slug_and_artificial] = row[variable_and_slug_and_artificial] = count;
    }
  }
  for (count = 0; count <= num_conditions; count++) {
    variable_matrix[count][count] = 1;
  }
}

double tableau(int row_num, int col_num) {
  int count;
  double data;

  /*消した行*/
  if (col[row_num] < 0)
    return 0;
  if (col_num <= num_variables) {
    data = 0;
    for (count=0; count <= num_conditions; count++) {
      data += variable_matrix[row_num][count] * variable[count][col_num];
    }
    return data;
  }

  data = variable_matrix[row_num][nonzero_row[col_num]];
  if (col_num <= variable_and_minus_slug) {
    return -data;
  }

  if (col_num <= variable_and_slug || row_num!=0) {
    return data;
  }
  return data + 1;
}

/*デモンストレーションのためシンプレックス表を出力*/
void writetableau (int row_pivot ,int col_pivot) {
  int count_1, count_2;

  for (count_1 = 0; count_1 <= num_conditions; count_1++) {
    if (col[count_1] >= 0) {
      printf("%2d: ",count_1);
      for (count_2 = 0; count_2 <= num_right_col; count_2++) {
        printf("%7.2f%c", tableau(count_1, count_2), (count_1 == row_pivot && count_2 == col_pivot) ? '*' : ' ');
      }
      printf("\n");
    }
  }
}

void pivot(int row_pivot, int col_pivot) {
  int count_1, count_2;
  double data;

  printf("ピボット位置 (%d, %d)\n",row_pivot, col_pivot);
  data = pivotcolumn[row_pivot];
  for (count_1 = 1; count_1 <= num_conditions; count_1++) {
    variable_matrix[row_pivot][count_1] /= data;
  }
  for (count_1 = 0; count_1 <= num_conditions; count_1++) {
    if (count_1 != row_pivot) {
      data = pivotcolumn[count_1];
      for (count_2 = 1; count_2 <= num_conditions; count_2++) {
        variable_matrix[count_1][count_2] -= variable_matrix[row_pivot][count_2] * data;
      }
    }
  }

  row[col[row_pivot]] = 0;
  col[row_pivot]=col_pivot;
  row[col_pivot]=row_pivot;
}

/*最小化*/
void minimize(void) {
  int count;
  int row_pivot,col_pivot;
  double divided_num,large;

  for ( ; ; ) {
    for (col_pivot = 1; col_pivot <= num_right_col; col_pivot++) {
      /*ピボット列col_pivotを見つける*/
      if (row[col_pivot] == 0) {
        pivotcolumn[0] = tableau(0, col_pivot);
        if (pivotcolumn[0] <- EPS)
          break;
      }
    }
    if (col_pivot > num_right_col)
      break;
    /*最小化完了*/
    large = LARGE;
    row_pivot = 0;
    /*ピボット行row_pivotを見つける*/
    for (count = 1; count <= num_conditions; count++) {
      pivotcolumn[count] = tableau(count, col_pivot);
      if (pivotcolumn[count] > EPS) {
        divided_num = tableau(count, 0) / pivotcolumn[count];
        if (divided_num < large) {
          row_pivot = count;
          large = divided_num;
        }
      }
    }
    if (row_pivot == 0) {
      printf("目的関数は下限がありません\n");
      exit(EXIT_SUCCESS);
    }
    writetableau(row_pivot, col_pivot);
    pivot(row_pivot, col_pivot);
  } //無限ループ終了

  writetableau(-1, -1);
  printf("最小値は%gです\n", -tableau(0, 0));
}

void phase1(void) {
  int count, base;
  double data;

  printf("フェーズ1\n");
  num_right_col = variable_and_slug_and_artificial;
  for (count = 0 ; count <= num_conditions; count++) {
    if (col[count] > variable_and_slug) {
      variable_matrix[0][count] = -1;
    }
  }
  minimize();
  if (tableau(0, 0) <- EPS) {
    printf("可能な解はありません\n");
    exit(EXIT_SUCCESS);
  }
  for (count = 1; count <= num_conditions; count++) {
    if (col[count] > variable_and_slug) {
      printf("条件 %d は冗長です\n",count);
      col[count] = -1;
    }
  }
  variable_matrix[0][0] = 1;

  for (count = 1; count <= num_conditions; count++) {
    variable_matrix[0][count] = 0;
  }

  for (count = 1; count <= num_conditions; count++) {
    base = col[count];
    data = purpose[base];
    if (base > 0 && base <= num_variables && data != 0) {
      for (base = 1; base <= num_conditions; base++) {
        variable_matrix[0][base] -= variable_matrix[count][base] * data;
      }
    }
  }
}
/*フェーズ２*/
void phase2(void) {
  int count;

  printf("フェーズ2\n");
  num_right_col = variable_and_slug;
  for (count = 0; count <= num_variables; count++) {
    variable[0][count] = purpose[count];
  }
  minimize();
}

void report(void) {
  int data, count;

  printf("0 でない変数の値:\n" );
  for (count = 1; count <= num_variables; count++) {
    data = row[count];
    if (data != 0) {
      printf("x[%d] = %g\n", count, tableau(data, 0));
    }
  }
}
