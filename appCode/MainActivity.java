package com.example.pedalpro12;

import android.content.Intent;
import android.os.Bundle;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import java.io.IOException;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.widget.Toast;

import android.content.SharedPreferences;

public class MainActivity extends AppCompatActivity {
    // variaveis dos elementos do aplicativo

    private final String ESP_URL = "http://192.168.4.1"; // IP do ESP8266
    private String ESP_URL2;

    private  TextView textView;
    private WebView webView;
    private WebSettings webSettings;
    private Toolbar toolbar;
    private final OkHttpClient client = new OkHttpClient();
   //funcao principal do aplicativo
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        webView = findViewById(R.id.webView);
        toolbar =findViewById(R.id.toolbar2);
        webSettings = webView.getSettings();

        textView = findViewById(R.id.textView2);

        carregarDados();


        setSupportActionBar(toolbar);
        //botao chama a funcao que obtem os dados no esp

        webView.setWebViewClient(new WebViewClient());

        webSettings.setJavaScriptEnabled(true);

        webSettings.setCacheMode(webSettings.LOAD_NO_CACHE);

        webSettings.setDomStorageEnabled(true);



    }
    //funcao que obtem os dados no esp

    private void contagemDeVoltas(){

        Request request = new Request.Builder().url(ESP_URL).build();

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        if (id == R.id.action_settings) {
            Toast.makeText(this, "Abrindo Configurações...", Toast.LENGTH_SHORT).show();
            Intent intent = new Intent(this, SettingsActivity.class);
            startActivity(intent);
            finish();
            return true;
        } else if (id == R.id.action_about) {
            Toast.makeText(this, "Sobre o App", Toast.LENGTH_SHORT).show();
            return true;
        } else if (id == R.id.action_exit) {
            finish(); // Fecha o app
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private void carregarDados() {
        SharedPreferences prefs = getSharedPreferences("MEUS_DADOS", MODE_PRIVATE);
        String nome = prefs.getString("NOME", "Nome não definido");
        textView.setText("http://" +nome);
        ESP_URL2 = textView.getText().toString();
        webView.loadUrl(ESP_URL2);

    }
}
