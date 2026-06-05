package Patrones;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

//Patrón Singleton que gestiona la ejecución de tareas en hilos
public class ThreadExecutor {
    private static ThreadExecutor instance;
    private ExecutorService executor;
    private static final int THREAD_POOL_SIZE = 10;

    private ThreadExecutor() {
        this.executor = Executors.newFixedThreadPool(THREAD_POOL_SIZE);
    }

    public static synchronized ThreadExecutor getInstance() {
        if (instance == null) {
            instance = new ThreadExecutor();
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

