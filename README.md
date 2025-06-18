# RTBOX
Criado por: Gabriel Fogli
Colaborador: Gustavo Auler
Última alteração: 06/06/2025

# Introdução
Este repositório é destinado ao uso do RT-BOX CE em conjunto com o LAUNCHXL-F28379D presente no GEP/UFMG.

Procedimento:

1 - Baixar o arquivo "base_RTBOX_CE_FULL_2025.rar" e extrair. 
2 - No Code Compose Studio: Project -> Import CCS Projects -> em "Select Search Directory" procure pela pasta do projeto (como exemplo, deve ser algo como: C:\Users\...\workspace_v12\base_RTBOX_CE_full_2025) -> Finish [ Nesta etapa o projeto deve aparecer no "Project explorer"].
  2.1 - Clicar no símbolo de martelo (Compile). Compilar o código. 

3 - O arquivo "main" é o .c com o mesmo nome que a pasta.

4 - Baixar e abrir o arquivo "base_RTBOXCE_tri_2025.plecs" [a versão do PLECS testada foi a 4.9.2]. 

  4.1-> Clicar em "Coder"-> "Coder options" -> "Target" -> símbolo do binóculo. Este passo é necessário para verificar a conexão entre o PC e o RT-BOX CE;
  4.2 -> Clicar em "External Mode" -> "Build" -> " Connect" [Este passo inicia a execução da simulação no PLECS];

5- Novamente, no CCS: Clicar em debug (símbolo do inseto).


