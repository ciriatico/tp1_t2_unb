#include "controladorasservico.h"

list<ElementoResultado> ComandoSQL::listaResultado;

//---------------------------------------------------------------------------
//Classe ErroPersistencia.

EErroPersistencia::EErroPersistencia(string mensagem){
        this->mensagem = mensagem;
}

string EErroPersistencia::what() {
        return mensagem;
}

//---------------------------------------------------------------------------
//Classe ElementoResultado.

void ElementoResultado::setNomeColuna(const string& nomeColuna) {
        this->nomeColuna = nomeColuna;
}

void ElementoResultado::setValorColuna(const string& valorColuna){
        this->valorColuna = valorColuna;
}

//---------------------------------------------------------------------------
//Classe ComandoSQL.

void ComandoSQL::conectar() {
      rc = sqlite3_open(nomeBancoDados, &bd);
      if( rc != SQLITE_OK )
        throw EErroPersistencia("Erro na conexao ao banco de dados");
}

void ComandoSQL::desconectar() {
      rc =  sqlite3_close(bd);
      if( rc != SQLITE_OK )
        throw EErroPersistencia("Erro na desconexao ao banco de dados");
}

void ComandoSQL::executar() {
        conectar();
        rc = sqlite3_exec(bd, comandoSQL.c_str(), callback, 0, &mensagem);
        if(rc != SQLITE_OK){
                sqlite3_free(mensagem);
                desconectar();
                throw EErroPersistencia("Erro na execucao do comando SQL");
        }
        desconectar();
}

int ComandoSQL::callback(void *NotUsed, int argc, char **valorColuna, char **nomeColuna){
      NotUsed=0;
      ElementoResultado elemento;
      int i;
      for(i=0; i<argc; i++){
        elemento.setNomeColuna(nomeColuna[i]);
        elemento.setValorColuna(valorColuna[i] ? valorColuna[i]: "NULL");
        listaResultado.push_front(elemento);
      }
      return 0;
}

//---------------------------------------------------------------------------
//Classe ComandoLerSenha.

ComandoLerSenha::ComandoLerSenha(Email email) {
        comandoSQL = "SELECT senha FROM usuarios WHERE email = '";
        comandoSQL += email.getValor();
        comandoSQL += "'";
}

string ComandoLerSenha::getResultado() {
        ElementoResultado resultado;
        string senha;

        //Remover senha;
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        senha = resultado.getValorColuna();

        return senha;
}

//---------------------------------------------------------------------------
//Classe ComandoCadastrarUsuario.

ComandoCadastrarUsuario::ComandoCadastrarUsuario(Usuario usuario) {
        comandoSQL = "INSERT INTO usuarios VALUES (";
        comandoSQL += "'" + usuario.getNome().getValor() + "', ";
        comandoSQL += "'" + usuario.getEmail().getValor() + "', ";
        comandoSQL += "'" + usuario.getSenha().getValor() + "', ";
        comandoSQL += "'" + usuario.getTelefone().getValor() + "')";
}

//---------------------------------------------------------------------------
//Classe ComandoDescadastrarUsuario.

ComandoDescadastrarUsuario::ComandoDescadastrarUsuario(Email email) {
        comandoSQL = "DELETE FROM usuarios WHERE email = '";
        comandoSQL += email.getValor();
        comandoSQL += "'";
}

//---------------------------------------------------------------------------
//Classe ComandoEditarUsuario.

ComandoEditarUsuario::ComandoEditarUsuario(Usuario usuario) {
        comandoSQL = "UPDATE usuarios ";
        comandoSQL += "SET nome = '" + usuario.getNome().getValor();
        comandoSQL += "', senha = '" + usuario.getSenha().getValor();
        comandoSQL += "', telefone = '" + usuario.getTelefone().getValor();
        comandoSQL += "' WHERE email = '" + usuario.getEmail().getValor() + "'";
}

//---------------------------------------------------------------------------
//Classe ComandoCadastrarImovel.

ComandoCadastrarImovel::ComandoCadastrarImovel(Imovel imovel, Email email) {

        comandoSQL = "INSERT INTO imoveis VALUES (";
        comandoSQL += "'" + imovel.getCodigo().getValor() + "', ";
        comandoSQL += "'" + to_string(imovel.getClasse().getValor()) + "', ";
        comandoSQL += "'" + imovel.getDescricao().getValor() + "', ";
        comandoSQL += "'" + imovel.getEndereco().getValor() + "', ";
        comandoSQL += "'" + imovel.getDataInicial().getValor() + "', ";
        comandoSQL += "'" + imovel.getDataFinal().getValor() + "', ";
        comandoSQL += "'" + to_string(imovel.getHospedes().getValor()) + "', ";
        comandoSQL += "'" + imovel.getValor().getValor() + "', ";
        comandoSQL += "'" + email.getValor() + "')";
}

//---------------------------------------------------------------------------
//Classe ComandoLerEmailImovel.

ComandoLerEmailImovel::ComandoLerEmailImovel(Codigo codigo) {
        comandoSQL = "SELECT email FROM imoveis WHERE codigo = '";
        comandoSQL += codigo.getValor();
        comandoSQL += "'";
}

string ComandoLerEmailImovel::getResultado() {
        ElementoResultado resultado;
        string email;

        //Remover email;
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        email = resultado.getValorColuna();

        return email;
}

//---------------------------------------------------------------------------
//Classe ComandoDescadastrarImovel.

ComandoDescadastrarImovel::ComandoDescadastrarImovel(Codigo codigo) {
        comandoSQL = "DELETE FROM imoveis WHERE codigo = '";
        comandoSQL += codigo.getValor();
        comandoSQL += "'";
}

//---------------------------------------------------------------------------
//Classe ComandoListarImoveis.

ComandoListarImoveis::ComandoListarImoveis() {
        comandoSQL = "SELECT * FROM imoveis";
}

list<Imovel> ComandoListarImoveis::getResultado(list<Email>* listaEmails) {
        ElementoResultado resultado;
        Imovel imovel;
        list<Imovel> listaImoveis;

        Codigo codigoImovel;
        Classe classeImovel;
        Descricao descricaoImovel;
        Endereco enderecoImovel;
        Data dataInicialImovel;
        Data dataFinalImovel;
        Numero hospedesImovel;
        Moeda valorImovel;

        Email emailUsuario;

        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");

        while (!listaResultado.empty()){
            resultado = listaResultado.back();
            listaResultado.pop_back();
            codigoImovel.setValor(resultado.getValorColuna());
            imovel.setCodigo(codigoImovel);

            resultado = listaResultado.back();
            listaResultado.pop_back();
            classeImovel.setValor(stoi(resultado.getValorColuna()));
            imovel.setClasse(classeImovel);

            resultado = listaResultado.back();
            listaResultado.pop_back();
            descricaoImovel.setValor(resultado.getValorColuna());
            imovel.setDescricao(descricaoImovel);

            resultado = listaResultado.back();
            listaResultado.pop_back();
            enderecoImovel.setValor(resultado.getValorColuna());
            imovel.setEndereco(enderecoImovel);

            resultado = listaResultado.back();
            listaResultado.pop_back();
            dataInicialImovel.setValor(resultado.getValorColuna());
            imovel.setDataInicial(dataInicialImovel);

            resultado = listaResultado.back();
            listaResultado.pop_back();
            dataFinalImovel.setValor(resultado.getValorColuna());
            imovel.setDataFinal(dataFinalImovel);

            resultado = listaResultado.back();
            listaResultado.pop_back();
            hospedesImovel.setValor(stoi(resultado.getValorColuna()));
            imovel.setHospedes(hospedesImovel);

            resultado = listaResultado.back();
            listaResultado.pop_back();
            valorImovel.setValor(resultado.getValorColuna());
            imovel.setValor(valorImovel);

            resultado = listaResultado.back();
            listaResultado.pop_back();
            emailUsuario.setValor(resultado.getValorColuna());

            listaImoveis.push_back(imovel);
            listaEmails->push_back(emailUsuario);
        }

        return listaImoveis;
}

//---------------------------------------------------------------------------
//Classe ComandoEditarImovel.

ComandoEditarImovel::ComandoEditarImovel(Imovel imovel) {
        comandoSQL = "UPDATE imoveis ";
        comandoSQL += "SET classe = '" + to_string(imovel.getClasse().getValor());
        comandoSQL += "', descricao = '" + imovel.getDescricao().getValor();
        comandoSQL += "', endereco = '" + imovel.getEndereco().getValor();
        comandoSQL += "', data_inicial = '" + imovel.getDataInicial().getValor();
        comandoSQL += "', data_final = '" + imovel.getDataFinal().getValor();
        comandoSQL += "', hospedes = '" + to_string(imovel.getHospedes().getValor());
        comandoSQL += "', valor = '" + imovel.getValor().getValor();
        comandoSQL += "' WHERE codigo = '" + imovel.getCodigo().getValor() + "'";
}

//---------------------------------------------------------------------------
//Classe ComandoDescadastrarImoveisPorEmail.

ComandoDescadastrarImoveisPorEmail::ComandoDescadastrarImoveisPorEmail(Email email) {
        comandoSQL = "DELETE FROM imoveis WHERE email = '";
        comandoSQL += email.getValor();
        comandoSQL += "'";
}

//---------------------------------------------------------------------------
//Classe ComandoPesquisarImovel.

ComandoPesquisarImovel::ComandoPesquisarImovel(Codigo codigo) {
        comandoSQL = "SELECT * FROM imoveis WHERE codigo = '";
        comandoSQL += codigo.getValor();
        comandoSQL += "'";
}

Imovel ComandoPesquisarImovel::getResultado(Email* emailUsuario) {
        ElementoResultado resultado;
        Imovel imovel;

        Codigo codigo;
        Classe classe;
        Descricao descricao;
        Endereco endereco;
        Data dataInicial;
        Data dataFinal;
        Numero hospedes;
        Moeda valor;
        Email email;

        // Remover codigo;
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        codigo.setValor(resultado.getValorColuna());
        imovel.setCodigo(codigo);

        // Remover classe;
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        classe.setValor(stoi(resultado.getValorColuna()));
        imovel.setClasse(classe);

        // Remover descricao;
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        descricao.setValor(resultado.getValorColuna());
        imovel.setDescricao(descricao);

        // Remover endereco;
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        endereco.setValor(resultado.getValorColuna());
        imovel.setEndereco(endereco);

        // Remover data_inicial;
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        dataInicial.setValor(resultado.getValorColuna());
        imovel.setDataInicial(dataInicial);

        // Remover data_final;
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        dataFinal.setValor(resultado.getValorColuna());
        imovel.setDataFinal(dataFinal);

        // Remover hospedes;
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        hospedes.setValor(stoi(resultado.getValorColuna()));
        imovel.setHospedes(hospedes);

        // Remover valor;
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        valor.setValor(resultado.getValorColuna());
        imovel.setValor(valor);

        // Remover email
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        emailUsuario->setValor(resultado.getValorColuna());

        return imovel;
}

//---------------------------------------------------------------------------
//Classe ComandoCadastrarProposta.

ComandoCadastrarProposta::ComandoCadastrarProposta(Proposta proposta, Email email, Codigo codigo) {

        comandoSQL = "INSERT INTO propostas VALUES (";
        comandoSQL += "'" + proposta.getCodigo().getValor() + "', ";
        comandoSQL += "'" + proposta.getDataInicial().getValor() + "', ";
        comandoSQL += "'" + proposta.getDataFinal().getValor() + "', ";
        comandoSQL += "'" + to_string(proposta.getHospedes().getValor()) + "', ";
        comandoSQL += "'" + proposta.getValor().getValor() + "', ";
        comandoSQL += "'" + email.getValor() + "', ";
        comandoSQL += "'" + codigo.getValor() + "')";
}

//---------------------------------------------------------------------------
//Classe ComandoLerEmailProposta.

ComandoLerEmailProposta::ComandoLerEmailProposta(Codigo codigo) {
        comandoSQL = "SELECT email FROM propostas WHERE codigo = '";
        comandoSQL += codigo.getValor();
        comandoSQL += "'";
}

string ComandoLerEmailProposta::getResultado() {
        ElementoResultado resultado;
        string email;

        //Remover email;
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        email = resultado.getValorColuna();

        return email;
}

//---------------------------------------------------------------------------
//Classe ComandoDescadastrarProposta.

ComandoDescadastrarProposta::ComandoDescadastrarProposta(Codigo codigo) {
        comandoSQL = "DELETE FROM propostas WHERE codigo = '";
        comandoSQL += codigo.getValor();
        comandoSQL += "'";
}

//---------------------------------------------------------------------------
//Classe ComandoPesquisarProposta.

ComandoPesquisarProposta::ComandoPesquisarProposta(Codigo codigo) {
        comandoSQL = "SELECT * FROM propostas WHERE codigo = '";
        comandoSQL += codigo.getValor();
        comandoSQL += "'";
}

Proposta ComandoPesquisarProposta::getResultado(Email* emailUsuario, Codigo* codigoImovel) {
        ElementoResultado resultado;
        Proposta proposta;

        Codigo codigo;
        Data dataInicial;
        Data dataFinal;
        Numero hospedes;
        Moeda valor;

        // Remover codigo;
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        codigo.setValor(resultado.getValorColuna());
        proposta.setCodigo(codigo);

        // Remover data_inicial;
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        dataInicial.setValor(resultado.getValorColuna());
        proposta.setDataInicial(dataInicial);

        // Remover data_final;
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        dataFinal.setValor(resultado.getValorColuna());
        proposta.setDataFinal(dataFinal);

        // Remover hospedes;
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        hospedes.setValor(stoi(resultado.getValorColuna()));
        proposta.setHospedes(hospedes);

        // Remover valor;
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        valor.setValor(resultado.getValorColuna());
        proposta.setValor(valor);

        // Remover email
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        emailUsuario->setValor(resultado.getValorColuna());

        // Remover codigo_imovel
        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");
        resultado = listaResultado.back();
        listaResultado.pop_back();
        codigoImovel->setValor(resultado.getValorColuna());

        return proposta;
}

//---------------------------------------------------------------------------
//Classe ComandoListarPropostas.

ComandoListarPropostas::ComandoListarPropostas() {
        comandoSQL = "SELECT * FROM propostas";
}

list<Proposta> ComandoListarPropostas::getResultado(list<Codigo>* listaCodigos, list<Email>* listaEmails) {
        ElementoResultado resultado;
        Proposta proposta;
        list<Proposta> listaPropostas;

        Codigo codigoProposta;
        Data dataInicialProposta;
        Data dataFinalProposta;
        Numero hospedesProposta;
        Moeda valorProposta;

        Codigo codigoImovel;
        Email emailUsuario;

        if (listaResultado.empty())
                throw EErroPersistencia("Lista de resultados vazia.");

        while (!listaResultado.empty()){
            resultado = listaResultado.back();
            listaResultado.pop_back();
            codigoProposta.setValor(resultado.getValorColuna());
            proposta.setCodigo(codigoProposta);

            resultado = listaResultado.back();
            listaResultado.pop_back();
            dataInicialProposta.setValor(resultado.getValorColuna());
            proposta.setDataInicial(dataInicialProposta);

            resultado = listaResultado.back();
            listaResultado.pop_back();
            dataFinalProposta.setValor(resultado.getValorColuna());
            proposta.setDataFinal(dataFinalProposta);

            resultado = listaResultado.back();
            listaResultado.pop_back();
            hospedesProposta.setValor(stoi(resultado.getValorColuna()));
            proposta.setHospedes(hospedesProposta);

            resultado = listaResultado.back();
            listaResultado.pop_back();
            valorProposta.setValor(resultado.getValorColuna());
            proposta.setValor(valorProposta);

            resultado = listaResultado.back();
            listaResultado.pop_back();
            emailUsuario.setValor(resultado.getValorColuna());

            resultado = listaResultado.back();
            listaResultado.pop_back();
            codigoImovel.setValor(resultado.getValorColuna());

            listaPropostas.push_back(proposta);
            listaEmails->push_back(emailUsuario);
            listaCodigos->push_back(codigoImovel);
        }

        return listaPropostas;
}

//---------------------------------------------------------------------------
//Classe ComandoDescadastrarPropostasPorEmail.

ComandoDescadastrarPropostasPorEmail::ComandoDescadastrarPropostasPorEmail(Email email) {
        comandoSQL = "DELETE FROM propostas WHERE email = '";
        comandoSQL += email.getValor();
        comandoSQL += "'";
}

//---------------------------------------------------------------------------
//Classe ComandoDescadastrarPropostasPorCodigoImovel.

ComandoDescadastrarPropostasPorCodigoImovel::ComandoDescadastrarPropostasPorCodigoImovel(Codigo codigo) {
        comandoSQL = "DELETE FROM propostas WHERE codigo_imovel = '";
        comandoSQL += codigo.getValor();
        comandoSQL += "'";
}

//---------------------------------------------------------------------------
//Classe ComandoDescadastrarPropostasDeImoveisDeUsuario.

ComandoDescadastrarPropostasDeImoveisDeUsuario::ComandoDescadastrarPropostasDeImoveisDeUsuario(Email email) {
        comandoSQL = "DELETE FROM propostas WHERE codigo IN ";
        comandoSQL += "(SELECT codigo FROM (SELECT propostas.codigo, imoveis.email ";
        comandoSQL += "FROM propostas, imoveis ";
        comandoSQL += "WHERE propostas.codigo_imovel = imoveis.codigo) WHERE email = '";
        comandoSQL += email.getValor();
        comandoSQL += "')";
}

//---------------------------------------------------------------------------
//Classe ComandoContarEntidade.

ComandoContarEntidade::ComandoContarEntidade(string entidade, string chave, string instancia) {
        comandoSQL = "SELECT count(";
        comandoSQL += chave;
        comandoSQL += ") FROM ";
        comandoSQL += entidade;
        comandoSQL += " WHERE ";
        comandoSQL += chave;
        comandoSQL += " = '";
        comandoSQL += instancia;
        comandoSQL += "' GROUP by ";
        comandoSQL += chave;
}

int ComandoContarEntidade::getResultado() {
        string contagem;
        ElementoResultado resultado;

        // Remover matricula;
        if (listaResultado.empty())
                return 0;

        resultado = listaResultado.back();
        listaResultado.pop_back();
        contagem = resultado.getValorColuna();

        return stoi(contagem);
}

//---------------------------------------------------------------------------
//Classe ComandoContarEntidadeDuplo.

ComandoContarEntidadeDuplo::ComandoContarEntidadeDuplo(string entidade, string chaveBase, string chaveUm, string chaveDois, string instanciaUm, string instanciaDois) {
        // ChaveBase ?? a chave que ser?? contada.
        // ChaveUm ser?? filtrada a partir de instanciaUm, e o mesmo para chaveDois e instanciaDois.

        comandoSQL = "SELECT count(";
        comandoSQL += chaveBase;
        comandoSQL += ") FROM ";
        comandoSQL += entidade;
        comandoSQL += " WHERE ";
        comandoSQL += chaveUm;
        comandoSQL += " = '";
        comandoSQL += instanciaUm;
        comandoSQL += "' AND ";
        comandoSQL += chaveDois;
        comandoSQL += " = '";
        comandoSQL += instanciaDois;
        comandoSQL += "'";
}

int ComandoContarEntidadeDuplo::getResultado() {
        string contagem;
        ElementoResultado resultado;

        // Remover matricula;
        if (listaResultado.empty())
                return 0;

        resultado = listaResultado.back();
        listaResultado.pop_back();
        contagem = resultado.getValorColuna();

        return stoi(contagem);
}

//---------------------------------------------------------------------------

bool checarUsuarioCadastrado(Email email){
    // Todo servi??o que precisa de usu??rio cadastrado precisa chegar primeiro se o usu??rio est?? cadastrado.
    // Isso evita usu??rio descadastrado continuando a executar servi??os.

    ComandoContarEntidade comandoContarUsuario("usuarios", "email", email.getValor());
    comandoContarUsuario.executar();

    if (comandoContarUsuario.getResultado() == 0)
        return false;

    return true;
}

//--------------------------------------------------------------------------------------------
// Implementa????es dos m??todos das classes controladoras da camada de servi??o.


//--------------------------------------------------------------------------------------------
// Implementa????es dos m??todos da classe controladora servi??o autentica????o.

bool CntrServicoAutenticacao::autenticar(Email email, Senha senha){

    ComandoLerSenha comandoLerSenha(email);

    try {
        comandoLerSenha.executar();

        string senhaRecuperada;

        senhaRecuperada = comandoLerSenha.getResultado();

        if (senhaRecuperada == senha.getValor())
            return true;

        return false;
    }
    catch(EErroPersistencia &exp) {
        return false;
    }
}

//--------------------------------------------------------------------------------------------
// Implementa????es dos m??todos da classe controladora servi??o pessoal.

bool CntrServicoPessoal::cadastrarUsuario(Usuario usuario){

    ComandoCadastrarUsuario comandoCadastrar(usuario);

    try{
        comandoCadastrar.executar();
        return true;
    }
    catch(EErroPersistencia &exp) {
        return false;
    }
}

bool CntrServicoPessoal::descadastrarUsuario(Email email){

    if (!checarUsuarioCadastrado(email))
        return false;

    // Ao descadastrar um usu??rio, ?? preciso descadastrar todas as propostas ligadas a seus im??veis.
    ComandoDescadastrarPropostasDeImoveisDeUsuario comandoDescadastrarPropostasDeImoveisDeUsuario(email);
    comandoDescadastrarPropostasDeImoveisDeUsuario.executar();

    // Ao descadastrar um usu??rio, ?? preciso descadastrat tamb??m todos seus an??ncios de im??veis.
    ComandoDescadastrarImoveisPorEmail comandoDescadastrarImoveis(email);
    comandoDescadastrarImoveis.executar();

    // As propostas veiculadas ao e-mail tamb??m s??o exclu??das.
    ComandoDescadastrarPropostasPorEmail comandoDescadastrarPropostas(email);
    comandoDescadastrarPropostas.executar();

    ComandoDescadastrarUsuario comandoDescadastrar(email);

    try{
        comandoDescadastrar.executar();
        return true;
    }
    catch(EErroPersistencia &exp) {
        return false;
    }
}

bool CntrServicoPessoal::editarUsuario(Usuario usuario){

    if (!checarUsuarioCadastrado(usuario.getEmail()))
    return false;

    ComandoEditarUsuario comandoEditarUsuario(usuario);

    try{
        comandoEditarUsuario.executar();
        return true;
    }
    catch(EErroPersistencia &exp) {
        return false;
    }
}

//--------------------------------------------------------------------------------------------
// Implementa????es dos m??todos da classe controladora servi??o im??veis.

Codigo CntrServicoImoveis::gerarCodigo(){

    int digitoChar;
    string digitoAleatorio;
    string stringCodigoAleatorio = "";
    Codigo codigoAleatorio;

    for (int i = 0; i < 5; i++)
    {
        digitoChar = rand() % 2;

        if (digitoChar)
            digitoAleatorio = 'A' + rand()%26;
        else
            digitoAleatorio = to_string(rand() % 10);

        stringCodigoAleatorio += digitoAleatorio;
    }

    codigoAleatorio.setValor(stringCodigoAleatorio);

    return codigoAleatorio;
}

bool CntrServicoImoveis::codigoValido(string entidade, Codigo codigo){
    int validadeCodigo;

    ComandoContarEntidade contarCodigoEntidade(entidade, "codigo", codigo.getValor());
    contarCodigoEntidade.executar();
    validadeCodigo = contarCodigoEntidade.getResultado();

    if (validadeCodigo == 0)
        return true;
    return false;
}

Codigo CntrServicoImoveis::gerarCodigoValido(string entidade){

    Codigo codigoAleatorio;
    bool codigoValidoEncontrado = false;

    while (!codigoValidoEncontrado){
        codigoAleatorio = gerarCodigo();

        if (codigoValido(entidade, codigoAleatorio))
            codigoValidoEncontrado = true;
    }

    return codigoAleatorio;
}

int CntrServicoImoveis::compararUnidadesData(int unidadeUm, int unidadeDois)
{
    if (unidadeDois > unidadeUm)
        return 3;
    if (unidadeDois < unidadeUm)
        return 1;
    if (unidadeDois == unidadeUm)
        return 2;
}

int CntrServicoImoveis::compararDatas(Data dataUm, Data dataDois)
{
    // Checar se a dataDois ?? maior ou igual ?? dataUm.

    char delim = '-';
	int posicaoPrimeiroDelim = 2;
	int posicaoSegundoDelim = 5;

	int diaUm, mesUm, anoUm;
	int diaDois, mesDois, anoDois;

    diaUm = stoi(dataUm.getValor().substr(0, 2));
	mesUm = stoi(dataUm.getValor().substr(posicaoPrimeiroDelim + 1, 2));
	anoUm = stoi(dataUm.getValor().substr(posicaoSegundoDelim + 1, 2));

    diaDois = stoi(dataDois.getValor().substr(0, 2));
	mesDois = stoi(dataDois.getValor().substr(posicaoPrimeiroDelim + 1, 2));
	anoDois = stoi(dataDois.getValor().substr(posicaoSegundoDelim + 1, 2));

    switch (compararUnidadesData(anoUm, anoDois)){
        case 3:
            return 3;
        case 1:
            return 1;
        case 2:
            switch (compararUnidadesData(mesUm, mesDois)){
                case 3:
                    return 3;
                case 1:
                    return 1;
                case 2:
                    return compararUnidadesData(diaUm, diaDois);
            }
    }
}

bool CntrServicoImoveis::checarIntervaloDatas(Data primeiroDataUm, Data primeiroDataDois, Data segundoDataUm, Data segundoDataDois)
{
    // Considerando o primeiro como a data ?? qual a segunda precisa se ajustar.
    // Assim, segundoDataUm precisa ser maior ou igual a primeiroDataUm.
    // E segundoDataDois precisa ser menor ou igual a primeiroDataDois.

    if (compararDatas(primeiroDataUm, segundoDataUm) == 1)
        return false;

    if (compararDatas(primeiroDataDois, segundoDataDois) == 3)
        return false;

    return true;
}

double CntrServicoImoveis::moedaParaDouble(Moeda moeda)
{
    string valor;
    valor = moeda.getValor();

	while (valor.find(".") != string::npos)
		valor.replace(valor.find("."), 1, "");

	while (valor.find(",") != string::npos)
		valor.replace(valor.find(","), 1, ".");

    // Convers??o do valor em formato certo para double.
	return stod(valor);
}

bool CntrServicoImoveis::validarPropostaCadastro(Proposta proposta, Codigo codigo){
    // ?? necess??rio checar que a proposta bate com o im??vel anunciado em termos de valor, an??ncio e data de in??cio e fim.
    Imovel imovel;
    Email emailUsuario;

    ComandoPesquisarImovel comandoPesquisarImovel(codigo);
    comandoPesquisarImovel.executar();

    imovel = comandoPesquisarImovel.getResultado(&emailUsuario);

    Data dataInicialImovel, dataFinalImovel;
    Data dataInicialProposta, dataFinalProposta;

    dataInicialImovel = imovel.getDataInicial();
    dataFinalImovel = imovel.getDataFinal();

    dataInicialProposta = proposta.getDataInicial();
    dataFinalProposta = proposta.getDataFinal();

    if (!checarIntervaloDatas(dataInicialImovel, dataFinalImovel, dataInicialProposta, dataFinalProposta))
        return false;

    if (moedaParaDouble(proposta.getValor()) < moedaParaDouble(imovel.getValor()))
        return false;

    if (proposta.getHospedes().getValor() > imovel.getHospedes().getValor())
        return false;

    return true;
}

bool CntrServicoImoveis::cadastrarImovel(Imovel imovel, Email email){

    if (!checarUsuarioCadastrado(email))
        return false;

    // Se usu??rio j?? tiver 5 im??veis cadastrados, n??o ?? poss??vel prosseguir o cadastro.
    ComandoContarEntidade comandoContarImoveis("imoveis", "email", email.getValor());
    comandoContarImoveis.executar();

    if (comandoContarImoveis.getResultado() >= 5)
        return false;

    // Gerar c??digo aleat??rio v??lido, considerando entidade im??veis.
    imovel.setCodigo(gerarCodigoValido("imoveis"));

    ComandoCadastrarImovel comandoCadastrarImovel(imovel, email);

    try{
        comandoCadastrarImovel.executar();
        return true;
    }
    catch(EErroPersistencia &exp) {
        return false;
    }
}

bool CntrServicoImoveis::consultarImovel(Imovel* imovel, Email* email){

    Imovel imovelConsultado;

    ComandoPesquisarImovel comandoPesquisarImovel(imovel->getCodigo());

    try{
        comandoPesquisarImovel.executar();

        imovelConsultado = comandoPesquisarImovel.getResultado(email);
    }
    catch(EErroPersistencia &exp) {
        return false;
    }

    imovel->setClasse(imovelConsultado.getClasse());
    imovel->setDescricao(imovelConsultado.getDescricao());
    imovel->setEndereco(imovelConsultado.getEndereco());
    imovel->setDataInicial(imovelConsultado.getDataInicial());
    imovel->setDataFinal(imovelConsultado.getDataFinal());
    imovel->setHospedes(imovelConsultado.getHospedes());
    imovel->setValor(imovelConsultado.getValor());

    return true;
}

bool CntrServicoImoveis::descadastrarImovel(Codigo codigo, Email email){

    if (!checarUsuarioCadastrado(email))
        return false;

    // Se o c??digo dado n??o existe, n??o h?? o que descadastrar.
    ComandoContarEntidade contarImovelDado("imoveis", "codigo", codigo.getValor());
    contarImovelDado.executar();

    if (contarImovelDado.getResultado() == 0)
        return false;

    // N??o ?? poss??vel usu??rio descadastrar im??vel que n??o anunciou.
    ComandoLerEmailImovel comandoEmailImovel(codigo);
    comandoEmailImovel.executar();

    if (comandoEmailImovel.getResultado() != email.getValor())
        return false;

    // Quando um im??vel ?? descadastrado, todas as propostas associadas a ele tamb??m devem ser descadastradas.
    ComandoDescadastrarPropostasPorCodigoImovel comandoDescadastrarPropostasPorCodigoImovel(codigo);
    comandoDescadastrarPropostasPorCodigoImovel.executar();

    ComandoDescadastrarImovel comandoDescadastrarImovel(codigo);

    try{
        comandoDescadastrarImovel.executar();
        return true;
    }
    catch(EErroPersistencia &exp) {
        return false;
    }
}

bool CntrServicoImoveis::listarImoveis(list<Imovel>* listaImoveis){

    list<Email> listaEmails;

    ComandoListarImoveis comandoListarImoveis;

    try{
        comandoListarImoveis.executar();
        *listaImoveis = comandoListarImoveis.getResultado(&listaEmails);
    }
    catch(EErroPersistencia &exp) {
        return false;
    }

    return true;
}

bool CntrServicoImoveis::editarImovel(Imovel imovel, Email email){

    if (!checarUsuarioCadastrado(email))
        return false;

    // Se o c??digo dado n??o existe, n??o h?? o que descadastrar.
    ComandoContarEntidade contarImovelDado("imoveis", "codigo", imovel.getCodigo().getValor());
    contarImovelDado.executar();

    if (contarImovelDado.getResultado() == 0)
        return false;

    // N??o ?? poss??vel usu??rio descadastrar im??vel que n??o anunciou.
    ComandoLerEmailImovel comandoEmailImovel(imovel.getCodigo());
    comandoEmailImovel.executar();

    if (comandoEmailImovel.getResultado() != email.getValor())
        return false;

    ComandoEditarImovel comandoEditarImovel(imovel);

    try{
        comandoEditarImovel.executar();
        return true;
    }
    catch(EErroPersistencia &exp) {
        return false;
    }
}

bool CntrServicoImoveis::cadastrarProposta(Proposta proposta, Email email, Codigo codigo){

    if (!checarUsuarioCadastrado(email))
        return false;

    // Uma proposta s?? pode ser feita para um im??vel cadastrado.
    ComandoContarEntidade comandoContarImoveis("imoveis", "codigo", codigo.getValor());
    comandoContarImoveis.executar();

    if (comandoContarImoveis.getResultado() == 0)
        return false;

    // Um usu??rio s?? pode fazer 1 proposta para cada im??vel.
    ComandoContarEntidadeDuplo comandoContarPropostasUsuario("propostas", "codigo", "codigo_imovel", "email", codigo.getValor(), email.getValor());
    comandoContarPropostasUsuario.executar();

    if (comandoContarPropostasUsuario.getResultado() >= 1)
        return false;

    // Processo de chegagem de que a proposta cadastrada bate com o im??vel em data, h??spedes e valor.
    if (!validarPropostaCadastro(proposta, codigo))
        return false;

    // Gerar c??digo aleat??rio v??lido, considerando entidade im??veis.
    proposta.setCodigo(gerarCodigoValido("propostas"));

    ComandoCadastrarProposta comandoCadastrarProposta(proposta, email, codigo);

    try{
        comandoCadastrarProposta.executar();
        return true;
    }
    catch(EErroPersistencia &exp) {
        return false;
    }
}

bool CntrServicoImoveis::descadastrarProposta(Codigo codigo, Email email){

    if(!checarUsuarioCadastrado(email))
        return false;

    // Se o c??digo dado n??o existe, n??o h?? o que descadastrar.
    ComandoContarEntidade contarPropostaDada("propostas", "codigo", codigo.getValor());
    contarPropostaDada.executar();

    if (contarPropostaDada.getResultado() == 0)
        return false;

    // N??o ?? poss??vel usu??rio descadastrar proposta que n??o cadastrou.
    ComandoLerEmailProposta comandoEmailProposta(codigo);
    comandoEmailProposta.executar();

    if (comandoEmailProposta.getResultado() != email.getValor())
        return false;

    ComandoDescadastrarProposta comandoDescadastrarProposta(codigo);

    try{
        comandoDescadastrarProposta.executar();
        return true;
    }
    catch(EErroPersistencia &exp) {
        return false;
    }
}

bool CntrServicoImoveis::consultarProposta(Email emailUsuario, Proposta* proposta, Email* email, Codigo* codigo){

    if (!checarUsuarioCadastrado(emailUsuario))
        return false;

    Proposta propostaConsultada;
    Email emailUsuarioConsultado;
    Codigo codigoImovelConsultado;

    ComandoPesquisarProposta comandoPesquisarProposta(proposta->getCodigo());

    try{
        comandoPesquisarProposta.executar();

        propostaConsultada = comandoPesquisarProposta.getResultado(&emailUsuarioConsultado, &codigoImovelConsultado);
    }
    catch(EErroPersistencia &exp) {
        return false;
    }

    // Se o e-mail vinculado ?? proposta for igual ao email do usu??rio, a proposta ?? dele e ele pode ter acesso.
    if (emailUsuarioConsultado.getValor() == emailUsuario.getValor()){
        *proposta = propostaConsultada;
        *email = emailUsuarioConsultado;
        *codigo = codigoImovelConsultado;

        return true;
    }

    // Se o e-mail vinculado ao im??vel da proposta for igual ao e-mail do usu??rio, a proposta ?? sobre um im??vel dele.
    Imovel imovelConsultado;
    Email emailImovelConsultado;

    ComandoPesquisarImovel comandopesquisarImovel(codigoImovelConsultado);

    try{
        comandopesquisarImovel.executar();

        imovelConsultado = comandopesquisarImovel.getResultado(&emailImovelConsultado);
    }
    catch(EErroPersistencia &exp) {
        return false;
    }

    if (emailImovelConsultado.getValor() == emailUsuario.getValor()){
        *proposta = propostaConsultada;
        *email = emailUsuarioConsultado;
        *codigo = codigoImovelConsultado;

        return true;
    }

    // Se a proposta n??o tiver sido feita pelo usu??rio ou em cima de um im??vel por ele cadastrado,
    // ele n??o pode ter acesso ?? proposta.
    return false;
}

bool CntrServicoImoveis::listarPropostas(Email emailUsuario, list<Proposta>* listaPropostasUsuario, list<Codigo>* listaCodigosImoveisUsuario){

    if(!checarUsuarioCadastrado(emailUsuario))
        return false;

    list<Email> listaEmails;
    list<Proposta> listaPropostas;
    list<Codigo> listaCodigos;

    ComandoListarPropostas comandoListarPropostas;

    try{
        comandoListarPropostas.executar();
        listaPropostas = comandoListarPropostas.getResultado(&listaCodigos, &listaEmails);
    }
    catch(EErroPersistencia &exp) {
        return false;
    }

    // A lista de propostas deve conter apenas aquelas feitas pelo usu??rio.
    list<Proposta>::iterator proposta = listaPropostas.begin();
    list<Codigo>::iterator codigo = listaCodigos.begin();

    for(list<Email>::iterator email = listaEmails.begin(); email != listaEmails.end(); email++){

        if (email->getValor() == emailUsuario.getValor()){
            listaPropostasUsuario->push_back(*proposta);
            listaCodigosImoveisUsuario->push_back(*codigo);
        }

        proposta++;
        codigo++;
    }

    return true;
}

bool CntrServicoImoveis::listarPropostasDeImovel(Email emailUsuario, Codigo codigoImovel, list<Proposta>* listaPropostasUsuario, list<Codigo>* listaCodigosImoveisUsuario){

    if(!checarUsuarioCadastrado(emailUsuario))
        return false;

    // Se o im??vel n??o existir, n??o h?? o que ver.
    ComandoContarEntidade contarImovelDado("imoveis", "codigo", codigoImovel.getValor());
    contarImovelDado.executar();

    if (contarImovelDado.getResultado() == 0)
        return false;

    // N??o ?? poss??vel ver propostas de im??vel que n??o ?? seu.
    Imovel imovelConsultado;
    Email emailImovelConsultado;

    ComandoPesquisarImovel comandoPesquisarImovel(codigoImovel);

    try{
        comandoPesquisarImovel.executar();
        imovelConsultado = comandoPesquisarImovel.getResultado(&emailImovelConsultado);
    }
    catch(EErroPersistencia &exp) {
        return false;
    }

    if (emailImovelConsultado.getValor() != emailUsuario.getValor())
        return false;

    list<Proposta> listaPropostas;
    list<Codigo> listaCodigosImoveis;
    list<Email> listaEmails;

    ComandoListarPropostas comandoListarPropostas;

    try{
        comandoListarPropostas.executar();
        listaPropostas = comandoListarPropostas.getResultado(&listaCodigosImoveis, &listaEmails);
    }
    catch(EErroPersistencia &exp) {
        return false;
    }

    list<Proposta>::iterator propostaLoop = listaPropostas.begin();

    for(list<Codigo>::iterator codigoLoop = listaCodigosImoveis.begin(); codigoLoop != listaCodigosImoveis.end(); codigoLoop++){

        if (codigoLoop->getValor() == codigoImovel.getValor()){
            listaPropostasUsuario->push_back(*propostaLoop);
            listaCodigosImoveisUsuario->push_back(*codigoLoop);
        }

        propostaLoop++;
    }

    return true;
}
