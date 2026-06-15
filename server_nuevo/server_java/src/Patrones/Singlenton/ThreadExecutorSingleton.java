package Patrones.Singlenton;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

//Patrón Singleton que gestiona la ejecución de tareas en hilos
public class ThreadExecutorSingleton {
    private static ThreadExecutorSingleton instance;
    private ExecutorService executor;
    private static final int THREAD_POOL_SIZE = 10;

    private ThreadExecutorSingleton() {
        this.executor = Executors.newFixedThreadPool(THREAD_POOL_SIZE);
    }

    public static synchronized ThreadExecutorSingleton getInstance() {
        if (instance == null) {
            instance = new ThreadExecutorSingleton();
        }
        return instance;
    }

    public void execute(Runnable task) {
        executor.execute(task);
    }

    public void shutdown() {
        if (executor != null && !executor.isShutdown()) {
            executor.shutdown();
        }
    }
}
