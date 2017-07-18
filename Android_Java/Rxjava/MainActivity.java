package com.yiqin.rxjava;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.MenuItem;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.Arrays;
import java.util.concurrent.TimeUnit;

import rx.Observable;
import rx.Subscriber;
import rx.android.schedulers.AndroidSchedulers;
import rx.functions.Func1;
import rx.schedulers.Schedulers;


public class MainActivity extends AppCompatActivity {

    private TextView mTextMessage;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mTextMessage = (TextView) findViewById(R.id.message);
        interval();

        range();

        flatmap();
        concatMap();

        throttleWithTimeout();//Debounce

        /**
         Distinct
         过滤掉重复的数据项，distinct的过滤规则是：只允许还没有发射过的数据项通过。

         ElementAt
         只发射第N项数据。elementAt和elementAtOrDefault默认不在任何特定的调度器上执行。

         Filter
         Filter操作符使用你指定的一个谓词函数测试数据项，只有通过测试的数据才会被发射。
         也就是说原始数据必须满足我们给的限制条件，才能被发射。 filter默认不在任何特定的调度器上执行。

         First
          如果你只对Observable发射的第一项数据，或者满足某个条件的第一项数据感兴趣，
         你可以使用First操作符。 first系列的这几个操作符默认不在任何特定的调度器上执行。

         takeFirst(Func1)：
         takeFirst与first类似，除了这一点：如果原始Observable没有发射任何满足条件的数据，
         first会抛出一个NoSuchElementException，takeFist会返回一个空的Observable（不调用onNext()但是会调用onCompleted）

         single()：
         single操作符也与first类似，但是如果原始Observable在完成之前不是正好发射一次数据，
         它会抛出一个NoSuchElementException

         Last
         只发射最后一项（或者满足某个条件的最后一项）数据。

         IgnoreElements
         IgnoreElements操作符忽略原始Observable发射的所有数据，只允许它的终止通知（onError或onCompleted）通过

         Sample/ThrottleFirst
         Sample (别名throttleLast)操作符定时查看一个Observable，然后发射自上次采样以来它最近发射的数据。
         ThrottleFirst操作符的功能类似，但不是发射采样期间的最近的数据，而是发射在那段时间内的第一项数据。

         Skip/SkipLast
         skip(int)：使用Skip操作符，你可以忽略Observable发射的前N项数据，只保留之后的数据

         Take/TakeLast
         take(int)：只发射前面的N项数据，然后发射完成通知，忽略剩余的数据。
         注意： 如果你对一个Observable使用take(n)（或它的同义词limit(n)）操作符，
         而那个Observable发射的数据少于N项，那么take操作生成的Observable不会抛异常或发射onError通知，
         在完成前它只会发射相同的少量数据。
         */


        /**
         *
         Combining Observables(Observable的组合操作符) 【视频教程】
         http://blog.csdn.net/jdsjlzx/article/details/52415615

         Merge
         在异步的世界经常会创建这样的场景，我们有多个来源但是只想有一个结果：多输入，单输出。
         RxJava的merge()方法将帮助你把两个甚至更多的Observables合并到他们发射的数据里。

         ZIP
         我们在处理多源时可能会带来这样一种场景：多从个Observables接收数据，处理它们，然后将它们合并成一个新的可观测序列来使用。
         RxJava有一个特殊的方法可以完成：zip()合并两个或者多个Observables发射出的数据项，根据指定的函数Func*变换它们，
         并发射一个新值。下图展示了zip()方法如何处理发射的“numbers”和“letters”然后将它们合并一个新的数据项

         Join
         前面两个方法，zip()和merge()方法作用在发射数据的范畴内，在决定如何操作值之前有些场景我们需要考虑时间的。
         RxJava的join()函数基于时间窗口将两个Observables发射的数据结合在一起。


         combineLatest

         RxJava的combineLatest()函数有点像zip()函数的特殊形式。正如我们已经学习的，zip()作用于最近未打包的两个Observables。
         相反，combineLatest()作用于最近发射的数据项：如果Observable1发射了A并且Observable2发射了B和C，
         combineLatest()将会分组处理AB和AC


         And,Then和When
         在将来还有一些zip()满足不了的场景。如复杂的架构，或者是仅仅为了个人爱好，
         你可以使用And/Then/When解决方案。它们在RxJava的joins包下，使用Pattern和Plan作为中介，将发射的数据集合并到一起。
         *
         *
         */
    }

    private void throttleWithTimeout() {
        // http://blog.csdn.net/jdsjlzx/article/details/51489061
    }

    private void flatmap() {
        // http://blog.csdn.net/jdsjlzx/article/details/51493552
    }


    //这也就解决了一个问题：既要是多个线程完成任务，又要保持任务的顺序。
    private void concatMap() {
        Observable.from(Arrays.asList(
                "http://www.baidu.com/",
                "http://www.google.com/",
                "https://www.bing.com/"))
                .concatMap(new Func1<String, Observable<String>>() {
                    @Override
                    public Observable<String> call(String s) {
                        return createIpObservableMultiThread(s);
                    }
                })
                .observeOn(AndroidSchedulers.mainThread());




    }

    // 获取ip
    private synchronized Observable<String> createIpObservableMultiThread(final String url) {
        return Observable
                .create(new Observable.OnSubscribe<String>() {
                    @Override
                    public void call(Subscriber<? super String> subscriber) {
//                        try {
//                            String ip = getIPByUrl(url);
//                            printLog(tvLogs, "Emit Data -> ", url + "->" + ip);
//                            subscriber.onNext(ip);
//                        } catch (MalformedURLException e) {
//                            e.printStackTrace();
//                            //subscriber.onError(e);
//                            subscriber.onNext(null);
//                        } catch (UnknownHostException e) {
//                            e.printStackTrace();
//                            //subscriber.onError(e);
//                            subscriber.onNext(null);
//                        }
                        subscriber.onCompleted();
                    }
                })
                .subscribeOn(Schedulers.io());
    }

    private BottomNavigationView.OnNavigationItemSelectedListener mOnNavigationItemSelectedListener
            = new BottomNavigationView.OnNavigationItemSelectedListener() {

        @Override
        public boolean onNavigationItemSelected(@NonNull MenuItem item) {
            switch (item.getItemId()) {
                case R.id.navigation_home:
                    mTextMessage.setText(R.string.title_home);
                    return true;
                case R.id.navigation_dashboard:
                    mTextMessage.setText(R.string.title_dashboard);
                    return true;
                case R.id.navigation_notifications:
                    mTextMessage.setText(R.string.title_notifications);
                    return true;
            }
            return false;
        }

    };

    public void interval(){
        Observable.interval(1, TimeUnit.SECONDS)
                .subscribe(new Subscriber<Long>() {
                    @Override
                    public void onCompleted() {
                        System.out.println("onCompleted" );
                    }
                    @Override
                    public void onError(Throwable e) {
                    }
                    @Override
                    public void onNext(Long aLong) {
                        System.out.println("interval:" + aLong);
                    }
                });


    }


    public void range(){
        Observable.range(100,10)
                .subscribe(new Subscriber(){


                    @Override
                    public void onCompleted() {
                        System.out.println("onCompleted:" );
                    }

                    @Override
                    public void onError(Throwable e) {
                        System.out.println("onCompleted:" );
                    }

                    @Override
                    public void onNext(Object o) {
                        System.out.println("onNext:"+o );
                    }


                });



    }







}
