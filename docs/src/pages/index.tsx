import type { ReactNode } from 'react';
import clsx from 'clsx';
import Link from '@docusaurus/Link';
import useDocusaurusContext from '@docusaurus/useDocusaurusContext';
import Layout from '@theme/Layout';
import styles from './index.module.css';

function HomepageHeader() {
  return (
    <header className={clsx(styles.heroBanner)}>
      <div className={styles.heroContainer}>
        <h1 className={styles.title}>R-Type Documentation</h1>
        <p className={styles.subtitle}>
          Learn how the R-Type project works / how to use it
        </p>
      </div>
    </header>
  );
}

function WhatIsRType() {
  return (
    <section className={styles.section}>
      <h2>⮕ What is R-Type?</h2>
      <p>
        <strong>R-Type</strong> is a multiplayer side-scrolling shooter inspired by the classic arcade game.
        This project involves implementing:
      </p>
      <ul>
        <li>A <strong>client/server architecture</strong></li>
        <li>An <strong>ECS (Entity-Component-System)</strong> engine</li>
        <li>A <strong>custom network protocol</strong></li>
        <li><strong>Real-time multiplayer gameplay</strong></li>
      </ul>
      <p>
        The server handles entities, movement, collisions, and authoritative game state.
        The client renders the game world, sends player inputs, and receives updates from the server.
      </p>
    </section>
  );
}

function ProjectGoals() {
  return (
    <section className={styles.section}>
          <h2>🧩 Project Goals</h2>
          <p>
            The goal of this R-Type project is to provide a modular and production-ready engine using real-world engineering practices:
          </p>
          <ul>
            <li>Robust ECS engine with modular systems</li>
            <li>Authoritative real-time multiplayer server</li>
            <li>Custom binary protocol optimized for low-latency communication</li>
            <li>Threaded ASIO networking model</li>
            <li>Flexible map system (entities, walls, hazards, spawners)</li>
            <li>Clean and maintainable C++ codebase</li>
          </ul>
        </section>
  );
}

export default function Home(): ReactNode {
  return (
    <Layout
      title="R-Type Documentation"
      description="A deep dive into the R-Type project, made by the Redemption Team."
    >
      <HomepageHeader />
      <main className={styles.main}>

        <WhatIsRType />
        <ProjectGoals />
      </main>
    </Layout>
  );
}
