defmodule ExNcurses.Server do
  use GenServer
  alias ExNcurses.Nif

  @moduledoc """
  This module serializes access to the ncurses NIF to avoid race conditions and
  handles enif_select notifications.
  """

  defmodule State do
    @moduledoc false
    defstruct pid: nil
  end

  def start_link(args) do
    GenServer.start_link(__MODULE__, args, name: __MODULE__)
  end

  @doc """
  Initialize the the ncurses library on the specified terminal.

  If "", the current terminal (the one with the IEx prompt)
  is used. You can also specify another terminal.
  """
  def initscr(termname) do
    GenServer.call(__MODULE__, {:initscr, termname})
  end

  @doc """
  Stop using ncurses.
  """
  def endwin() do
    GenServer.call(__MODULE__, :endwin)
  end

  @doc """
  Invoke a ncurses C function
  """
  def invoke(name, args \\ {}) do
    GenServer.call(__MODULE__, {:invoke, name, args})
  end

  @doc """
  Listen for events from ncurses
  """
  def listen() do
    GenServer.call(__MODULE__, :listen)
  end

  @doc """
  Stop listening for events from ncurses
  """
  def stop_listening() do
    GenServer.call(__MODULE__, :stop_listening)
  end

  def init(_args) do
    {:ok, %State{}}
  end

  def handle_call({:initscr, termname}, _from, state) do
    # Handling the case where termname specifies the terminal with the console:
    #
    # The NIF swaps out stdin in the call to initscr so that the IEx console or
    # any Erlang console code doesn't processes keys that should go to ncurses.
    # In order for this to work reliably, the stdin filehandle cannot be in
    # use.  Specifically, it can't be submitted to a syscall since the swapping
    # procedure causes an error on any in progress syscalls.  To fix this, tell
    # the Erlang scheduler to only allow one OS thread during the swap. This
    # would normally be an unforgivable offense to the Erlang VM, but
    # presumably ncurses isn't being initialized frequently.
    :erlang.system_flag(:multi_scheduling, :block)
    rc = Nif.initscr(termname)
    :erlang.system_flag(:multi_scheduling, :unblock)

    # Assuming initialization was ok, start polling stdin for key presses
    if rc == :ok, do: Nif.poll()

    {:reply, rc, state}
  end

  def handle_call(:endwin, _from, state) do
    :erlang.system_flag(:multi_scheduling, :block)
    rc = Nif.endwin()
    :erlang.system_flag(:multi_scheduling, :unblock)
    {:reply, rc, state}
  end

  def handle_call({:invoke, name, args}, _from, state) do
    {:reply, Nif.invoke(name, args), state}
  end

  def handle_call(:listen, {pid, _ref}, state) do
    {:reply, :ok, %{state | pid: pid}}
  end

  def handle_call(:stop_listening, _from, state) do
    {:reply, :ok, %{state | pid: nil}}
  end

  def handle_info({:select, _res, _ref, :ready_input}, state) do
    key = Nif.read()
    maybe_send(state.pid, {:ex_ncurses, :key, key})
    {:noreply, state}
  end

  defp maybe_send(nil, _message), do: :ok
  defp maybe_send(pid, message), do: send(pid, message)
end
