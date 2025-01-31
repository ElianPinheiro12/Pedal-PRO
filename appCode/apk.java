package com.example.pedalpro12;

import android.os.Bundle;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;
import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import java.io.IOException;

public class MainActivity extends AppCompatActivity {
    // variaveis dos elementos do aplicativo
    private TextView txtResposta;
    private Button btnConectar;
    private final String ESP_URL = "http://192.168.4.1"; // IP do ESP8266
    private EditText editText;

    private WebView webView;

    private WebSettings webSettings;

    private final OkHttpClient client = new OkHttpClient();
   //funcao principal do aplicativo
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        webView = findViewById(R.id.webView);
        webSettings = webView.getSettings();
        txtResposta = findViewById(R.id.txtResposta);
        btnConectar = findViewById(R.id.btnConectar);
        editText = findViewById(R.id.editTextIP);
        //botao chama a funcao que obtem os dados no esp
        btnConectar.setOnClickListener(v -> obterDadosDoESP());

        webView.setWebViewClient(new WebViewClient());

        webSettings.setJavaScriptEnabled(true);

        webSettings.setCacheMode(webSettings.LOAD_NO_CACHE);

        webSettings.setDomStorageEnabled(true);


        webView.loadUrl(ESP_URL);
    }
    //funcao que obtem os dados no esp
    private void obterDadosDoESP() {
        String texto = editText.getText().toString();
        Request request = new Request.Builder().url("http://"+texto).build();

        client.newCall(request).enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                runOnUiThread(() -> txtResposta.setText("Erro de conexão: " + e.getMessage()));
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                if (response.isSuccessful()) {
                    String resposta = response.body().string();
                    runOnUiThread(() -> txtResposta.setText("Resposta: " + resposta));
                }
            }
        });
    }
    private void contagemDeVoltas(){

        Request request = new Request.Builder().url(ESP_URL).build();

    }
}
