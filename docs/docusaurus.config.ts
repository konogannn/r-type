import {themes as prismThemes} from 'prism-react-renderer';
import type {Config} from '@docusaurus/types';
import type * as Preset from '@docusaurus/preset-classic';

const config: Config = {
  title: 'R-Type Docs',
  tagline: 'Documentation for the R-Type project',
  favicon: 'img/favicon.ico',

  future: {
    v4: true, // Improve compatibility with the upcoming Docusaurus v4
  },

  url: 'https://konogannn.github.io',
  baseUrl: '/r-type/',

  organizationName: 'konogannn',
  projectName: 'r-type',

  onBrokenLinks: 'throw',

  i18n: {
    defaultLocale: 'en',
    locales: ['en'],
  },

  presets: [
    [
      'classic',
      {
        docs: {
          sidebarPath: './sidebars.ts',
          editUrl:
            'https://github.com/konogannn/r-type/tree/main/',
        },
        theme: {
          customCss: './src/css/custom.css',
        },
      } satisfies Preset.Options,
    ],
  ],

  themeConfig: {
    // Replace with your project's social card
    colorMode: {
      respectPrefersColorScheme: true,
    },
    navbar: {
      title: 'R-Type Docs',
      logo: {
        alt: 'R-Type Docs Logo',
        src: 'img/logo.svg',
      },
      items: [
        {
          type: 'docSidebar',
          sidebarId: 'technicalSidebar',
          position: 'left',
          label: 'Technical Documentation',
        },
        {
          href: 'https://github.com/konogannn/r-type',
          label: 'GitHub',
          position: 'right',
        },
      ],
    },
    footer: {
      style: 'dark',
      links: [
        {
          title: 'Contact us',
          items: [
            {
              label: 'Alex Augereau',
              href: 'mailto:alex.augereau@epitech.eu'
            },
            {
              label: 'Jules Fayet',
              href: 'mailto:jules.fayet@epitech.eu'
            },
            {
              label: 'Konogan Pineau',
              href: 'mailto:konogan.pineau@epitech.eu'
            },
            {
              label: 'Nolan Papa',
              href: 'mailto:nolan.papa@epitech.eu'
            },
            {
              label: 'Titouan Bouillot Bachelier',
              href: 'mailto:titouan.bouillot-bachelier@epitech.eu'
            }
          ],
        }
      ],
      copyright: `Copyright © ${new Date().getFullYear()} Redemption Team. Built with Docusaurus.`,
    },
    prism: {
      theme: prismThemes.github,
      darkTheme: prismThemes.dracula,
    },
  } satisfies Preset.ThemeConfig,
};

export default config;
