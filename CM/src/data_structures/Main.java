package data_structures;



import java.util.Random;

import java.util.concurrent.CyclicBarrier;

import data_structures.implementation.CoarseGrainedList;
import data_structures.implementation.CoarseGrainedTree;
import data_structures.implementation.FineGrainedList;
import data_structures.implementation.FineGrainedTree;
import data_structures.implementation.LockFreeList;
import data_structures.implementation.LockFreeTree;



public class Main {

	private static final boolean ALLOW_DOUBLE_ELEMENTS = true;

	private static final String CGL = "cgl";
	private static final String CGT = "cgt";
	private static final String FGL = "fgl";
	private static final String FGT = "fgt";
	private static final String LFL = "lfl";
	private static final String LFT = "lft";


	private static long computeSeed(int param1, int param2, int param3) {
	    // make a unique number from the three parameters
	    long result = param2;
	    result <<= 16;
	    result |= (long) param1;
	    result <<= 16;
	    result |= (long) param3;
	    return result;
	}

	private static void permute(int[] array, long seed) {
		Random random = new Random(seed);

		for (int i = 0; i < array.length; i++) {
			int r = random.nextInt(array.length);
			int swapped = array[i];
			array[i] = array[r];
			array[r] = swapped;
		}
	}

	private static void createWorkDataWithoutDoubles(int[] itemsToAdd,
			int[] itemsToRemove, long seed) {
		for (int i = 0; i < itemsToAdd.length; i++) {
			itemsToAdd[i] = i;
			itemsToRemove[i] = i;
		}

		permute(itemsToAdd, seed);
		permute(itemsToRemove, seed + 1);
	}

	private static void createWorkDataWithDoubles(int[] itemsToAdd,
			int[] itemsToRemove, long seed) {
		Random random = new Random(seed);
		for (int i = 0; i < itemsToAdd.length; i++) {
			int nextRandom = random.nextInt();
			itemsToAdd[i] = nextRandom;
			itemsToRemove[i] = nextRandom;
		}

		permute(itemsToRemove, seed + 1);
	}

	private static void createWorkData(int[] itemsToAdd, 
			int[] itemsToRemove, long seed) {
		if (ALLOW_DOUBLE_ELEMENTS) {
			createWorkDataWithDoubles(itemsToAdd, itemsToRemove,
				seed);
		} else {
			createWorkDataWithoutDoubles(itemsToAdd, itemsToRemove,
				seed);
		}
	
	}

	private static void startThreads(Sorted<Integer> sorted, int nrThreads,
			int nrItems, int workTime, long seed) throws
			InterruptedException {
		int[] itemsToAdd = new int[nrItems];
		int[] itemsToRemove = new int[nrItems];
		createWorkData(itemsToAdd, itemsToRemove, seed);

		WorkerThread[] workerThreads = new WorkerThread[nrThreads];
		CyclicBarrier barrier = new CyclicBarrier(nrThreads);

		for (int i = 0; i < nrThreads; i++) {
			workerThreads[i] = new WorkerThread(i, sorted, nrItems
					/ nrThreads, itemsToAdd, itemsToRemove, workTime, barrier);
		}

		long start = System.currentTimeMillis();
		for (int i = 0; i < nrThreads; i++) {
			workerThreads[i].start();
		}

		for (int i = 0; i < nrThreads; i++) {
			workerThreads[i].join();
		}
		long end = System.currentTimeMillis();

		System.out.println("data structure after removal (should be empty):");
		System.out.println(sorted);
		System.out.println();
		System.out.printf("time: %d ms\n\n", end - start);
	}

	private static void performWork(String dataStructure, int nrThreads,
		    int nrItems, int workTime, long seed)
			throws InterruptedException {
		Sorted<Integer> sorted = null;
		if (dataStructure.equals(CGL)) {
			sorted = new CoarseGrainedList<Integer>();
		} else if (dataStructure.equals(CGT)) {
			sorted = new CoarseGrainedTree<Integer>();
		} else if (dataStructure.equals(FGL)) {
			sorted = new FineGrainedList<Integer>();
		} else if (dataStructure.equals(FGT)) {
			sorted = new FineGrainedTree<Integer>();
		} else if (dataStructure.equals(LFL)) {
			sorted = new LockFreeList<Integer>();
		} else if (dataStructure.equals(LFT)) {
			sorted = new LockFreeTree<Integer>();
		} else {
			exitWithError();
		}

		startThreads(sorted, nrThreads, nrItems, workTime, seed);
	}

	private static void exitWithError() {
		System.out
				.println("test_data_structures <data_structure> <nrThreads> <nrItems> <workTime>");
		System.out.println("  where:");
		System.out.printf("    <data_structure> in {%s, %s, %s, %s, %s, %s}\n",
				CGL, CGT, FGL, FGT, LFL, LFT);
		System.out.println("    <nrThreads> is a number > 0");
		System.out.println("    <nrItems> is a number > 0");
		System.out.println("    <workTime> is a number >= 0 (micro seconds)");
		System.exit(1);
	}

	public static void main(String[] args) throws InterruptedException {
		if (args.length != 4) {
			exitWithError();
		}

		String dataStructure = args[0];
		int nrThreads = Integer.parseInt(args[1]);
		if (nrThreads < 1) {
			exitWithError();
		}

		int nrItems = Integer.parseInt(args[2]);
		if (nrItems < 1) {
			exitWithError();
		}

		if (nrItems % nrThreads != 0) {
			System.out.println("nrItems should be divisible by nrThreads");
			System.exit(1);
		}

		int workTime = Integer.parseInt(args[3]);
		if (workTime < 0) {
			exitWithError();
		}

		long seed = computeSeed(nrThreads, nrItems, workTime);

		performWork(dataStructure, nrThreads, nrItems, workTime, seed);
	}
}
