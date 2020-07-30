#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include"afd.h"

int main(){
	FILE* input_file = fopen("t.txt", "r");

	int estado_atual     = estado_inicial;
	int ultimo_final     = 0;
	int c_inicio_leitura = 0;
	int c_posicao_atual  = 0;
	int c_ult_fin_recon  = 0;
	char input_atual     = '\0';

	fseek(input_file, 0, SEEK_END);
	int tamanho_cadeia = ftell(input_file);
	fseek(input_file, 0, SEEK_SET);

	do{
		input_atual = fgetc(input_file);

		int proximo_estado = estados[estado_atual][selecionarPosicao(input_atual)];
		if(proximo_estado<=0){
			reiniciarLeitura(input_atual, &ultimo_final, &proximo_estado, &estado_atual, &c_inicio_leitura, &c_posicao_atual, &c_ult_fin_recon, input_file);
			if(input_atual != EOF)
				printf("\n");
		}else{
			continuarLeitura(input_atual, &estado_atual, &proximo_estado, &c_posicao_atual, &ultimo_final, &c_ult_fin_recon);
		}

	}while(input_atual != EOF || c_posicao_atual < tamanho_cadeia);
	
	fclose(input_file);
return 0;
}